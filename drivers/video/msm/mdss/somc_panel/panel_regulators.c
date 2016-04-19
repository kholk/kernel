/*
 * Restructured unified display panel driver for Xperia Open Devices
 *                  *** Special VREG handling ***
 *
 * This driver is based on various SoMC implementations found in
 * copyleft archives for various devices.
 *
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 * Copyright (c) Sony Mobile Communications Inc. All rights reserved.
 * Copyright (C) 2014-2016, AngeloGioacchino Del Regno <kholk11@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/gpio.h>
#include <linux/regulator/qpnp-labibb-regulator.h>

#include "../mdss_dsi.h"
#include "somc_panels.h"

static int somc_panel_vregs_init(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct mdss_panel_specific_pdata *spec_pdata = NULL;
	int min_uV, max_uV = 0;

	spec_pdata = ctrl->spec_pdata;
	if (!spec_pdata) {
		pr_err("%s: FATAL: NULL SoMC panel data!!\n", __func__);
		return -EINVAL;
	}

	/*
	 * Set lab/ibb voltage.
	 */
	min_uV = spec_pdata->lab_output_voltage;
	max_uV = min_uV;
	rc = regulator_set_voltage(ctrl->lab, min_uV, max_uV);
	if (rc) {
		pr_err("%s: Unable to configure of lab voltage.\n", __func__);
		return rc;
	}
	min_uV = spec_pdata->ibb_output_voltage;
	max_uV = min_uV;
	rc = regulator_set_voltage(ctrl->ibb, min_uV, max_uV);
	if (rc) {
		pr_err("%s: Unable to configure of ibb voltage.\n", __func__);
		return rc;
	}

	/**
	 * Set lab/ibb current max
	 */
	if (spec_pdata->lab_current_max_enable) {
		rc = qpnp_lab_set_current_max(ctrl->lab,
				spec_pdata->lab_current_max);
		if (rc) {
			pr_err("%s: Unable to configure of lab current_max.\n",
								__func__);
			return rc;
		}
	}
	if (spec_pdata->ibb_current_max_enable) {
		rc = qpnp_ibb_set_current_max(ctrl->ibb,
				spec_pdata->ibb_current_max);
		if (rc) {
			pr_err("%s: Unable to configure of ibb current_max.\n",
								__func__);
			return rc;
		}
	}
}

static int somc_panel_vreg_ctrl(struct mdss_dsi_ctrl_pdata *ctrl, int enable)
{
	int rc;

	if (!ctrl->panel_bias_vreg || !ctrl->lab || !ctrl->ibb)
		return -EINVAL;

	pr_debug("%s: ndx=%d enable=%d\n", __func__, ctrl->ndx, enable);

	if (enable) {
		rc = regulator_enable(ctrl->lab);
		if (rc) {
			pr_err("%s: enable failed for lab regulator\n",
							__func__);
			return rc;
		}
		rc = regulator_enable(ctrl->ibb);
		if (rc) {
			pr_err("%s: enable failed for ibb regulator\n",
							__func__);
			regulator_disable(ctrl->lab);
			return rc;
		}

	} else {
		rc = regulator_disable(ctrl->lab);
		if (rc) {
			pr_err("%s: disable failed for lab regulator\n",
							__func__);
			return rc;
		}

		rc = regulator_disable(ctrl->ibb);
		if (rc) {
			pr_err("%s: disable failed for ibb regulator\n",
							__func__);
			return rc;
		}
	}

	return 0;
}

int somc_panel_regulators_init(struct mdss_dsi_ctrl_pdata *ctrl_pdata)
{
	ctrl_pdata->spec_pdata->vreg_init = somc_panel_vregs_init;
	ctrl_pdata->spec_pdata->vreg_ctrl = somc_panel_vregs_ctrl;

	return 0;
}
