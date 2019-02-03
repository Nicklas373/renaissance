/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#include <linux/sched.h>

#define MAX_CURRENT_UA 1000000
#define MAX_RAILS 5
#define MAX_THRESHOLD 2
#define MONITOR_ALL_TSENS -1
#define BYTES_PER_FUSE_ROW  8
#define MAX_EFUSE_VALUE  16
#define THERM_SECURE_BITE_CMD 8

static struct msm_thermal_data msm_thermal_info;
static struct delayed_work check_temp_work;
static bool core_control_enabled;
static uint32_t cpus_offlined;
static DEFINE_MUTEX(core_control_mutex);
static struct kobject *cc_kobj;
static struct task_struct *hotplug_task;
static struct task_struct *freq_mitigation_task;
static struct task_struct *thermal_monitor_task;
static struct completion hotplug_notify_complete;
static struct completion freq_mitigation_complete;
static struct completion thermal_monitor_complete;

static int enabled;
static int polling_enabled;
static int rails_cnt;
static int psm_rails_cnt;
static int ocr_rail_cnt;
static int limit_idx;
static int limit_idx_low;
static int limit_idx_high;
static int max_tsens_num;
static struct cpufreq_frequency_table *table;
static uint32_t usefreq;
static int freq_table_get;
static bool vdd_rstr_enabled;
static bool vdd_rstr_nodes_called;
static bool vdd_rstr_probed;
static bool psm_enabled;
static bool psm_nodes_called;
static bool psm_probed;
static bool hotplug_enabled;
static bool freq_mitigation_enabled;
static bool ocr_enabled;
static bool ocr_nodes_called;
static bool ocr_probed;
static bool interrupt_mode_enable;
static bool msm_thermal_probed;
static bool therm_reset_enabled;
static int *tsens_id_map;
static DEFINE_MUTEX(vdd_rstr_mutex);
static DEFINE_MUTEX(psm_mutex);
static DEFINE_MUTEX(ocr_mutex);
static uint32_t min_freq_limit;
static uint32_t default_cpu_temp_limit;
static bool default_temp_limit_enabled;
static bool default_temp_limit_probed;
static bool default_temp_limit_nodes_called;

unsigned int temp_threshold = 70;
module_param(temp_threshold, int, 0755);

static struct thermal_info {
	uint32_t cpuinfo_max_freq;
	uint32_t limited_max_freq;
	unsigned int safe_diff;
	bool throttling;
	bool pending_change;
} info = {
	.cpuinfo_max_freq = LONG_MAX,
	.limited_max_freq = LONG_MAX,
	.safe_diff = 5,
	.throttling = false,
	.pending_change = false,
};

enum thermal_freqs {
	FREQ_HELL		= 787200,
	FREQ_VERY_HOT		= 1094400,
	FREQ_HOT		= 1344000,
	FREQ_WARM		= 1593600,
};

enum threshold_levels {
	LEVEL_HELL		= 1 << 4,
	LEVEL_VERY_HOT		= 1 << 3,
	LEVEL_HOT		= 1 << 2,
};

static struct msm_thermal_data msm_thermal_info;

static struct delayed_work check_temp_work;

unsigned short get_threshold(void)
{
	return temp_threshold;
}

static int msm_thermal_cpufreq_callback(struct notifier_block *nfb,
		unsigned long event, void *data)
{
	struct cpufreq_policy *policy = data;

	if (event != CPUFREQ_ADJUST && !info.pending_change)
		return 0;

	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq,
		info.limited_max_freq);

	return 0;
}

static struct notifier_block msm_thermal_cpufreq_notifier = {
	.notifier_call = msm_thermal_cpufreq_callback,
};

static void limit_cpu_freqs(uint32_t max_freq)
{
	unsigned int cpu;

	if (info.limited_max_freq == max_freq)
		return;

	info.limited_max_freq = max_freq;

	info.pending_change = true;

	get_online_cpus();
	for_each_online_cpu(cpu)
	{
		cpufreq_update_policy(cpu);
		pr_info("%s: Setting cpu%d max frequency to %d\n",
				KBUILD_MODNAME, cpu, info.limited_max_freq);
	}
	put_online_cpus();

	info.pending_change = false;
}

static void check_temp(struct work_struct *work)
{
	struct tsens_device tsens_dev;
	uint32_t freq = 0;
	long temp = 0;

	tsens_dev.sensor_num = msm_thermal_info.sensor_id;
	tsens_get_temp(&tsens_dev, &temp);

	if (info.throttling)
	{
		if (temp < (temp_threshold - info.safe_diff))
		{
			limit_cpu_freqs(info.cpuinfo_max_freq);
			info.throttling = false;
			goto reschedule;
		}
	}

	if (temp >= temp_threshold + LEVEL_HELL)
		freq = FREQ_HELL;
	else if (temp >= temp_threshold + LEVEL_VERY_HOT)
		freq = FREQ_VERY_HOT;
	else if (temp >= temp_threshold + LEVEL_HOT)
		freq = FREQ_HOT;
	else if (temp > temp_threshold)
		freq = FREQ_WARM;

	if (freq)
	{
		limit_cpu_freqs(freq);

		if (!info.throttling)
			info.throttling = true;
	}

reschedule:
	schedule_delayed_work_on(0, &check_temp_work, msecs_to_jiffies(250));
}

static int __devinit msm_thermal_dev_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device_node *node = pdev->dev.of_node;
	struct msm_thermal_data data;

	memset(&data, 0, sizeof(struct msm_thermal_data));

	ret = of_property_read_u32(node, "qcom,sensor-id", &data.sensor_id);
	if (ret)
		return ret;

	WARN_ON(data.sensor_id >= TSENS_MAX_SENSORS);

        memcpy(&msm_thermal_info, &data, sizeof(struct msm_thermal_data));

        INIT_DELAYED_WORK(&check_temp_work, check_temp);
        schedule_delayed_work_on(0, &check_temp_work, 5);

	cpufreq_register_notifier(&msm_thermal_cpufreq_notifier,
			CPUFREQ_POLICY_NOTIFIER);

	return ret;
}

static int msm_thermal_dev_remove(struct platform_device *pdev)
{
	cpufreq_unregister_notifier(&msm_thermal_cpufreq_notifier,
                        CPUFREQ_POLICY_NOTIFIER);
	return 0;
}

static struct of_device_id msm_thermal_match_table[] = {
	{.compatible = "qcom,msm-thermal"},
	{},
};

static struct platform_driver msm_thermal_device_driver = {
	.probe = msm_thermal_dev_probe,
	.remove = msm_thermal_dev_remove,
	.driver = {
		.name = "msm-thermal",
		.owner = THIS_MODULE,
		.of_match_table = msm_thermal_match_table,
	},
};

static int __init msm_thermal_device_init(void)
{
	return platform_driver_register(&msm_thermal_device_driver);
}

static void __exit msm_thermal_device_exit(void)
{
	platform_driver_unregister(&msm_thermal_device_driver);
}

late_initcall(msm_thermal_device_init);
module_exit(msm_thermal_device_exit);
