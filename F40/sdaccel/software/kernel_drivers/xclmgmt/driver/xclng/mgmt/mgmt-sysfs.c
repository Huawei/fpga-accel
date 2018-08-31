/*
 * sysfs for the device attributes.
 *
 * Copyright (C) 2016-2017 Xilinx, Inc. All rights reserved.
 *
 * Authors:
 *    Lizhi Hou <lizhih@xilinx.com>
 *    Umang Parekh <umang.parekh@xilinx.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>

#include "mgmt-core.h"

enum {
	SYSMON_NAME,
	MB_NAME
};

static ssize_t instance_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", lro->instance);
}
static DEVICE_ATTR_RO(instance);

//Microblaze attributes

static ssize_t mb_version_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_VERSION);

	return sprintf(buf, "%d\n", val);
}
static DEVICE_ATTR_RO(mb_version);

static ssize_t mb_id_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_ID);

	return sprintf(buf, "0x%x\n", val);
}
static DEVICE_ATTR_RO(mb_id);

static ssize_t mb_status_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_STATUS);

	return sprintf(buf, "0x%x\n", val);
}
static DEVICE_ATTR_RO(mb_status);

static ssize_t mb_error_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_ERR);

	return sprintf(buf, "0x%x\n", val);
}
static DEVICE_ATTR_RO(mb_error);

static ssize_t mb_capability_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_CAP);

	return sprintf(buf, "0x%x\n", val);
}
static DEVICE_ATTR_RO(mb_capability);

static ssize_t mb_power_checksum_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_POW_CHK);

	return sprintf(buf, "%d\n", val);
}
static DEVICE_ATTR_RO(mb_power_checksum);

static ssize_t mb_pause_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_CTL);

	return sprintf(buf, "%d\n", !!(val & MB_CTL_MASK_PAUSE));
}

static ssize_t mb_pause_store(struct device *dev, struct device_attribute *da,
	const char *buf, size_t count)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	if (kstrtou32(buf, 10, &val) == -EINVAL || val > 1) {
		return -EINVAL;
	}

	val = val ? MB_CTL_MASK_PAUSE : 0;
	MGMT_WRITE_REG32(lro, MB_REG_CTL, val);

	return count;
}
static DEVICE_ATTR_RW(mb_pause);

static struct attribute *mb_attrs[] = {
	&dev_attr_mb_version.attr,
	&dev_attr_mb_id.attr,
	&dev_attr_mb_status.attr,
	&dev_attr_mb_error.attr,
	&dev_attr_mb_capability.attr,
	&dev_attr_mb_power_checksum.attr,
	&dev_attr_mb_pause.attr,
	NULL,
};

static struct attribute_group mb_attr_group = {
	.attrs = mb_attrs,
};

static ssize_t show_mb_pw(struct device *dev, struct device_attribute *da,
	char *buf)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;

	val = MGMT_READ_REG32(lro, MB_REG_CURR_BASE +
	    attr->index * sizeof(u32));

	return sprintf(buf, "%d\n", val);
}

static ssize_t show_sysmon(struct device *dev, struct device_attribute *da,
	char *buf)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	u32 val;
	int32_t	tmp;

	val = MGMT_READ_REG32(lro, SYSMON_BASE + attr->index);

	switch (attr->index) {
	case SYSMON_TEMP:
	case SYSMON_TEMP_MAX:
	case SYSMON_TEMP_MIN:
		tmp = SYSMON_TO_MILLDEGREE(val);
		break;
	default:
		tmp = SYSMON_TO_MILLVOLT(val);
		break;
	}

	return sprintf(buf, "%d\n", tmp);
}

static ssize_t show_name(struct device *dev, struct device_attribute *da,
	char *buf)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	ssize_t	sz = 0;

	switch (attr->index) {
	case SYSMON_NAME:
		sz = sprintf(buf, "%s\n", XCLMGMT_SYSMON_HWMON_NAME);
		break;

	case MB_NAME:
		sz = sprintf(buf, "%s\n", XCLMGMT_MB_HWMON_NAME);
		break;
	}

	return sz;
}

static SENSOR_DEVICE_ATTR(temp1_input, 0444, show_sysmon, NULL, SYSMON_TEMP);
static SENSOR_DEVICE_ATTR(temp1_highest, 0444, show_sysmon, NULL,
	SYSMON_TEMP_MAX);
static SENSOR_DEVICE_ATTR(temp1_lowest, 0444, show_sysmon, NULL,
	SYSMON_TEMP_MIN);

static SENSOR_DEVICE_ATTR(in0_input, 0444, show_sysmon, NULL, SYSMON_VCCINT);
static SENSOR_DEVICE_ATTR(in0_highest, 0444, show_sysmon, NULL,
	SYSMON_VCCINT_MAX);
static SENSOR_DEVICE_ATTR(in0_lowest, 0444, show_sysmon, NULL,
	SYSMON_VCCINT_MIN);
static SENSOR_DEVICE_ATTR(in1_input, 0444, show_sysmon, NULL, SYSMON_VCCAUX);
static SENSOR_DEVICE_ATTR(in1_highest, 0444, show_sysmon, NULL,
	SYSMON_VCCAUX_MAX);
static SENSOR_DEVICE_ATTR(in1_lowest, 0444, show_sysmon, NULL,
	SYSMON_VCCAUX_MIN);
static SENSOR_DEVICE_ATTR(in2_input, 0444, show_sysmon, NULL, SYSMON_VCCBRAM);
static SENSOR_DEVICE_ATTR(in2_highest, 0444, show_sysmon, NULL,
	SYSMON_VCCBRAM_MAX);
static SENSOR_DEVICE_ATTR(in2_lowest, 0444, show_sysmon, NULL,
	SYSMON_VCCBRAM_MIN);

static struct attribute *hwmon_sysmon_attributes[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp1_highest.dev_attr.attr,
	&sensor_dev_attr_temp1_lowest.dev_attr.attr,
	&sensor_dev_attr_in0_input.dev_attr.attr,
	&sensor_dev_attr_in0_highest.dev_attr.attr,
	&sensor_dev_attr_in0_lowest.dev_attr.attr,
	&sensor_dev_attr_in1_input.dev_attr.attr,
	&sensor_dev_attr_in1_highest.dev_attr.attr,
	&sensor_dev_attr_in1_lowest.dev_attr.attr,
	&sensor_dev_attr_in2_input.dev_attr.attr,
	&sensor_dev_attr_in2_highest.dev_attr.attr,
	&sensor_dev_attr_in2_lowest.dev_attr.attr,
	NULL
};

static const struct attribute_group hwmon_sysmon_attrgroup = {
	.attrs = hwmon_sysmon_attributes,
};

static SENSOR_DEVICE_ATTR(curr1_highest, 0444, show_mb_pw, NULL, 0);
static SENSOR_DEVICE_ATTR(curr1_average, 0444, show_mb_pw, NULL, 1);
static SENSOR_DEVICE_ATTR(curr1_input, 0444, show_mb_pw, NULL, 2);
static SENSOR_DEVICE_ATTR(curr2_highest, 0444, show_mb_pw, NULL, 3);
static SENSOR_DEVICE_ATTR(curr2_average, 0444, show_mb_pw, NULL, 4);
static SENSOR_DEVICE_ATTR(curr2_input, 0444, show_mb_pw, NULL, 5);
static SENSOR_DEVICE_ATTR(curr3_highest, 0444, show_mb_pw, NULL, 6);
static SENSOR_DEVICE_ATTR(curr3_average, 0444, show_mb_pw, NULL, 7);
static SENSOR_DEVICE_ATTR(curr3_input, 0444, show_mb_pw, NULL, 8);
static SENSOR_DEVICE_ATTR(curr4_highest, 0444, show_mb_pw, NULL, 9);
static SENSOR_DEVICE_ATTR(curr4_average, 0444, show_mb_pw, NULL, 10);
static SENSOR_DEVICE_ATTR(curr4_input, 0444, show_mb_pw, NULL, 11);
static SENSOR_DEVICE_ATTR(curr5_highest, 0444, show_mb_pw, NULL, 12);
static SENSOR_DEVICE_ATTR(curr5_average, 0444, show_mb_pw, NULL, 13);
static SENSOR_DEVICE_ATTR(curr5_input, 0444, show_mb_pw, NULL, 14);
static SENSOR_DEVICE_ATTR(curr6_highest, 0444, show_mb_pw, NULL, 15);
static SENSOR_DEVICE_ATTR(curr6_average, 0444, show_mb_pw, NULL, 16);
static SENSOR_DEVICE_ATTR(curr6_input, 0444, show_mb_pw, NULL, 17);

static struct attribute *hwmon_mb_attributes[] = {
	&sensor_dev_attr_curr1_highest.dev_attr.attr,
	&sensor_dev_attr_curr1_average.dev_attr.attr,
	&sensor_dev_attr_curr1_input.dev_attr.attr,
	&sensor_dev_attr_curr2_highest.dev_attr.attr,
	&sensor_dev_attr_curr2_average.dev_attr.attr,
	&sensor_dev_attr_curr2_input.dev_attr.attr,
	&sensor_dev_attr_curr3_highest.dev_attr.attr,
	&sensor_dev_attr_curr3_average.dev_attr.attr,
	&sensor_dev_attr_curr3_input.dev_attr.attr,
	&sensor_dev_attr_curr4_highest.dev_attr.attr,
	&sensor_dev_attr_curr4_average.dev_attr.attr,
	&sensor_dev_attr_curr4_input.dev_attr.attr,
	&sensor_dev_attr_curr5_highest.dev_attr.attr,
	&sensor_dev_attr_curr5_average.dev_attr.attr,
	&sensor_dev_attr_curr5_input.dev_attr.attr,
	&sensor_dev_attr_curr6_highest.dev_attr.attr,
	&sensor_dev_attr_curr6_average.dev_attr.attr,
	&sensor_dev_attr_curr6_input.dev_attr.attr,
	NULL
};

static const struct attribute_group hwmon_mb_attrgroup = {
	.attrs = hwmon_mb_attributes,
};

static struct sensor_device_attribute sysmon_name_attr =
	SENSOR_ATTR(name, 0444, show_name, NULL, SYSMON_NAME);
static struct sensor_device_attribute mb_name_attr =
	SENSOR_ATTR(name, 0444, show_name, NULL, MB_NAME);

static void mgmt_sysfs_destroy_sysmon(struct device *dev)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);

	device_remove_file(dev, &sysmon_name_attr.dev_attr);
	sysfs_remove_group(&lro->sysmon_hwmon_dev->kobj,
		&hwmon_sysmon_attrgroup);

	hwmon_device_unregister(lro->sysmon_hwmon_dev);
	lro->sysmon_hwmon_dev = NULL;
}

static int mgmt_sysfs_create_sysmon(struct device *dev)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	int err;

	lro->sysmon_hwmon_dev = hwmon_device_register(dev);
	if (IS_ERR(lro->sysmon_hwmon_dev)) {
		err = PTR_ERR(lro->sysmon_hwmon_dev);
		dev_err(dev, "register sysmon hwmon failed: 0x%x", err);
		goto hwmon_reg_failed;
	}
	dev_set_drvdata(lro->sysmon_hwmon_dev, lro);
	err = device_create_file(lro->sysmon_hwmon_dev,
		&sysmon_name_attr.dev_attr);
	if (err) {
		dev_err(dev, "create attr name failed: 0x%x", err);
		goto create_name_failed;
	}

	err = sysfs_create_group(&lro->sysmon_hwmon_dev->kobj,
		&hwmon_sysmon_attrgroup);
	if (err) {
		dev_err(dev, "create pw group failed: 0x%x", err);
		goto create_grp_failed;
	}

	return 0;

create_grp_failed:
	device_remove_file(dev, &sysmon_name_attr.dev_attr);
create_name_failed:
	hwmon_device_unregister(lro->sysmon_hwmon_dev);
	lro->sysmon_hwmon_dev = NULL;
hwmon_reg_failed:
	return err;
}

static void mgmt_sysfs_destroy_mb(struct device *dev)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);

	device_remove_file(dev, &mb_name_attr.dev_attr);
	if (lro->mb_cap & MB_CAP_MASK_PM) {
		sysfs_remove_group(&lro->mb_hwmon_dev->kobj,
		    &hwmon_mb_attrgroup);
	}

	hwmon_device_unregister(lro->mb_hwmon_dev);
	lro->mb_hwmon_dev = NULL;

	sysfs_remove_group(&dev->kobj, &mb_attr_group);
}

static int mgmt_sysfs_create_mb(struct device *dev)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	int err;

	err = sysfs_create_group(&dev->kobj, &mb_attr_group);
	if (err) {
		dev_err(dev, "create mb attrs failed: 0x%x", err);
		goto create_attr_failed;
	}
	lro->mb_hwmon_dev = hwmon_device_register(dev);
	if (IS_ERR(lro->mb_hwmon_dev)) {
		err = PTR_ERR(lro->mb_hwmon_dev);
		dev_err(dev, "register mb hwmon failed: 0x%x", err);
		goto hwmon_reg_failed;
	}

	dev_set_drvdata(lro->mb_hwmon_dev, lro);
	err = device_create_file(lro->mb_hwmon_dev,
		&mb_name_attr.dev_attr);
	if (err) {
		dev_err(dev, "create attr name failed: 0x%x", err);
		goto create_name_failed;
	}

	if (lro->mb_cap & MB_CAP_MASK_PM) {
		err = sysfs_create_group(&lro->mb_hwmon_dev->kobj,
		    &hwmon_mb_attrgroup);
		if (err) {
			dev_err(dev, "create pw group failed: 0x%x", err);
			goto create_pw_failed;
		}
	}

	return 0;

create_pw_failed:
	device_remove_file(dev, &mb_name_attr.dev_attr);
create_name_failed:
	hwmon_device_unregister(lro->mb_hwmon_dev);
	lro->mb_hwmon_dev = NULL;
hwmon_reg_failed:
	sysfs_remove_group(&dev->kobj, &mb_attr_group);
create_attr_failed:
	return err;
}

int mgmt_init_sysfs(struct device *dev)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);
	int err;

	err = device_create_file(dev, &dev_attr_instance);
	if (err) {
		goto fail_exit;
	}

	err = mgmt_sysfs_create_sysmon(dev);
	if (err) {
		goto create_sysmon_failed;
	}

	if (lro->mb_on) {
		err = mgmt_sysfs_create_mb(dev);
		if (err) {
			goto create_mb_failed;
		}
	}

	return 0;

create_mb_failed:
	mgmt_sysfs_destroy_sysmon(dev);
create_sysmon_failed:
	device_remove_file(dev, &dev_attr_instance);
fail_exit:
	return err;
}

void mgmt_fini_sysfs(struct device *dev)
{
	struct xclmgmt_dev *lro = dev_get_drvdata(dev);

	device_remove_file(dev, &dev_attr_instance);

	mgmt_sysfs_destroy_sysmon(dev);
	if (lro->mb_on) {
		mgmt_sysfs_destroy_mb(dev);
	}
}

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
