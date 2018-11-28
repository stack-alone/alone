/*
 * config_json.h
 *
 *  Created on: Nov 28, 2018
 *      Author: zhaoyu <9641354@qq.com>
 */

#ifndef CONFIG_JSON_H_
#define CONFIG_JSON_H_

extern zend_class_entry *alone_config_json_ce;

alone_config_t *alone_config_json_instance(alone_config_t *this_ptr, zval *filename);
PHP_JSON_API int php_json_decode_ex(zval *return_value, char *str, size_t str_len, zend_long options, zend_long depth);

PHP_METHOD(Alone_Config_Json,__construct);
PHP_METHOD(Alone_Config_Json,read);
ALONE_STARTUP_FUNCTION(config_json);

#endif /* CONFIG_JSON_H_ */
