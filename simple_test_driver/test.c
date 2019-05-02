#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/of_device.h>
#include <linux/string.h>


static int test_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int r = -ENODEV;
	dev_info(&client->dev, "%s : test i2c driver probed\n",__func__);
	dev_info(&client->dev, "%s : device-tree node address %p\n",__func__,client->dev.of_node);
	dev_info(&client->dev, "%s : my i2c slave address is 0x%x, name is '%s'\n",__func__,(unsigned)client->addr,client->name);
	if(client->dev.of_node)
	{
		const char *str;
		if(!(r=of_property_read_string(client->dev.of_node,"test-string-property",&str)))
			dev_info(&client->dev, "%s: Found text '%s' from test-string-property in device-tree\n",__func__,str);
		else
			dev_err(&client->dev, "%s: of_property_read_string(\"test-string-property\") returned error %d\n",__func__,r);
	}
	else
		dev_err(&client->dev, "%s: of_property_read_string(\"test-string-property\") returned error %d\n",__func__,r);
	return r;
}

static const struct i2c_device_id test_ids[] = {
	{ "test", },
	{ }
};
MODULE_DEVICE_TABLE(i2c, test_ids);

static const struct of_device_id test_of_match[] = {
	{
		.compatible = "rohm,test",
	},
	{ },
};
MODULE_DEVICE_TABLE(of, test_of_match);

static struct i2c_driver test_drv = {
	.driver = {
		.name	= "test",
		.of_match_table = of_match_ptr(test_of_match),
	},
	.probe		= test_probe,
	.id_table	= test_ids,
};
module_i2c_driver(test_drv);


MODULE_DESCRIPTION("test i2c driver to see how BBB env works");
MODULE_LICENSE("GPL");

