/*
 * Copyright (C) 2017 Broadcom
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/thermal.h>

/*
 * In stingray thermal IO memory,
 * Total Number of available TMONs MASK is at offset 0
 * temperature registers BASE is at 4 byte offset.
 * Each TMON temperature register size is 4.
 */
#define SR_TMON_TEMP_BASE(id)   ((id) * 0x4)

#define SR_TMON_MAX_LIST        6

struct sr_tmon {
	struct thermal_zone_device *tz;
	unsigned int crit_temp;
	unsigned int tmon_id;
	struct sr_thermal *priv;
};

struct sr_thermal {
	void __iomem *regs;
	unsigned int max_crit_temp;
	struct sr_tmon tmon[SR_TMON_MAX_LIST];
};

static int sr_get_temp(void *data, int *temp)
{
	struct sr_tmon *tmon = data;
	struct sr_thermal *sr_thermal = tmon->priv;

	*temp = readl(sr_thermal->regs + SR_TMON_TEMP_BASE(tmon->tmon_id));

	return 0;
}

static const struct thermal_zone_of_device_ops sr_tz_ops = {
	.get_temp = sr_get_temp,
};

static int sr_thermal_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct sr_thermal *sr_thermal;
	struct sr_tmon *tmon;
	struct resource *res;
	uint32_t sr_tmon_list = 0;
	unsigned int i;
	int ret;

	sr_thermal = devm_kzalloc(dev, sizeof(*sr_thermal), GFP_KERNEL);
	if (!sr_thermal)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	sr_thermal->regs = (void __iomem *)devm_memremap(&pdev->dev, res->start,
							 resource_size(res),
							 MEMREMAP_WB);
	if (IS_ERR(sr_thermal->regs)) {
		dev_err(dev, "failed to get io address\n");
		return PTR_ERR(sr_thermal->regs);
	}

	ret = device_property_read_u32(dev, "brcm,tmon-mask", &sr_tmon_list);
	if (ret)
		return ret;

	tmon = sr_thermal->tmon;
	for (i = 0; i < SR_TMON_MAX_LIST; i++, tmon++) {

		if (!(sr_tmon_list & BIT(i)))
			continue;

		/* Flush temperature registers */
		writel(0, sr_thermal->regs + SR_TMON_TEMP_BASE(i));
		tmon->tmon_id = i;
		tmon->priv = sr_thermal;
		tmon->tz = devm_thermal_zone_of_sensor_register(dev, i, tmon,
								&sr_tz_ops);
		if (IS_ERR(tmon->tz))
			return PTR_ERR(tmon->tz);

		dev_info(dev, "thermal sensor %d registered\n", i);
	}
	platform_set_drvdata(pdev, sr_thermal);

	return 0;
}

static int sr_thermal_remove(struct platform_device *pdev)
{
	struct sr_thermal *sr_thermal = platform_get_drvdata(pdev);
	unsigned int i;
	struct sr_tmon *tmon;

	tmon = sr_thermal->tmon;
	for (i = 0; i < SR_TMON_MAX_LIST; i++, tmon++)
		if (tmon->tz)
			devm_thermal_zone_of_sensor_unregister(&pdev->dev,
							       tmon->tz);

	return 0;
}

static const struct of_device_id sr_thermal_of_match[] = {
	{ .compatible = "brcm,sr-thermal", },
	{},
};
MODULE_DEVICE_TABLE(of, sr_thermal_of_match);

static const struct acpi_device_id sr_thermal_acpi_ids[] = {
	{ .id = "BRCM0500" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(acpi, sr_thermal_acpi_ids);

static struct platform_driver sr_thermal_driver = {
	.probe		= sr_thermal_probe,
	.remove		= sr_thermal_remove,
	.driver = {
		.name = "sr-thermal",
		.of_match_table = sr_thermal_of_match,
		.acpi_match_table = ACPI_PTR(sr_thermal_acpi_ids),
	},
};
module_platform_driver(sr_thermal_driver);

MODULE_AUTHOR("Pramod Kumar <pramod.kumar@broadcom.com>");
MODULE_DESCRIPTION("Stingray thermal driver");
MODULE_LICENSE("GPL v2");
