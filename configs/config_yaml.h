#ifndef _ALONE_CONFIG_YAML_H
#define _ALONE_CONFIG_YAML_H


extern zend_class_entry *alone_config_yaml_ce;

alone_config_t *alone_config_yaml_instance(alone_config_t *this_ptr, zval *filename);

//PHP_METHOD(Alone_Config_Yaml,__construct);
PHP_METHOD(Alone_Config_Yaml,read);
ALONE_STARTUP_FUNCTION(config_yaml);

#endif
