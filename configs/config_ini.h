#ifndef _ALONE_CONFIG_INI_H
#define _ALONE_CONFIG_INI_H


extern zend_class_entry *alone_config_ini_ce;
static void alone_simple_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr);
static void alone_ini_parser_cb_with_sections(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr);
alone_config_t *alone_config_ini_instance(alone_config_t *this_ptr, zval *filename);

//PHP_METHOD(Alone_Config_Ini,__construct);
PHP_METHOD(Alone_Config_Ini,read);
ALONE_STARTUP_FUNCTION(config_ini);

#endif
