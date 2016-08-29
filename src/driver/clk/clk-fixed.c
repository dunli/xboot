/*
 * driver/clk/clk-fixed.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <clk/clk.h>

static void clk_fixed_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_fixed_get_parent(struct clk_t * clk)
{
	return NULL;
}

static void clk_fixed_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_fixed_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_fixed_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_fixed_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_fixed_t * fclk = (struct clk_fixed_t *)clk->priv;
	return fclk->rate;
}

bool_t register_clk_fixed(struct device_t ** device, struct clk_fixed_t * fclk)
{
	struct clk_t * clk;

	if(!fclk || !fclk->name)
		return FALSE;

	if(search_clk(fclk->name))
		return FALSE;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
		return FALSE;

	clk->name = fclk->name;
	clk->type = CLK_TYPE_FIXED;
	kref_init(&clk->count);
	clk->set_parent = clk_fixed_set_parent;
	clk->get_parent = clk_fixed_get_parent;
	clk->set_enable = clk_fixed_set_enable;
	clk->get_enable = clk_fixed_get_enable;
	clk->set_rate = clk_fixed_set_rate;
	clk->get_rate = clk_fixed_get_rate;
	clk->priv = fclk;

	if(!register_clk(device, clk))
	{
		free(clk);
		return FALSE;
	}
	return TRUE;
}

bool_t unregister_clk_fixed(struct clk_fixed_t * fclk)
{
	struct clk_t * clk;

	if(!fclk || !fclk->name)
		return FALSE;

	clk = search_clk(fclk->name);
	if(!clk)
		return FALSE;

	if(unregister_clk(clk))
	{
		free(clk);
		return TRUE;
	}
	return FALSE;
}

static struct device_t * clk_fixed_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_fixed_t * fclk;
	struct device_t * dev;
	char * name = dt_read_string(n, "name", NULL);
	u64_t rate = dt_read_u64(n, "rate", 0);

	if(!name || rate <= 0)
		return NULL;

	fclk = malloc(sizeof(struct clk_fixed_t));
	if(!fclk)
		return NULL;

	fclk->name = strdup(name);
	fclk->rate = rate;

	if(!register_clk_fixed(&dev, fclk))
	{
		free(fclk->name);
		free(fclk);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void clk_fixed_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_fixed_t * fclk = (struct clk_fixed_t *)clk->priv;

	if(fclk && unregister_clk_fixed(fclk))
	{
		free(fclk->name);
		free(fclk);
	}
}

static void clk_fixed_suspend(struct device_t * dev)
{
}

static void clk_fixed_resume(struct device_t * dev)
{
}

static struct driver_t clk_fixed = {
	.name		= "clk-fixed",
	.probe		= clk_fixed_probe,
	.remove		= clk_fixed_remove,
	.suspend	= clk_fixed_suspend,
	.resume		= clk_fixed_resume,
};

static __init void clk_fixed_driver_init(void)
{
	register_driver(&clk_fixed);
}

static __exit void clk_fixed_driver_exit(void)
{
	unregister_driver(&clk_fixed);
}

driver_initcall(clk_fixed_driver_init);
driver_exitcall(clk_fixed_driver_exit);
