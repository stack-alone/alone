#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "Zend/zend_interfaces.h"

#include "php_alone.h"
#include "alone_config.h"

#include "configs/config_ini.h"
#include "configs/config_yaml.h"



zend_class_entry *alone_config_ce;

ZEND_BEGIN_ARG_INFO_EX(global_factory_arg, 0, 0, 1)
    ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()


PHP_METHOD(Alone_Config,factory) {

	zval *filename = NULL;
	alone_config_t *instance = NULL;
	alone_config_t zconfig = {{0}};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &filename) == FAILURE) {
		if (Z_STRVAL_P(filename) == NULL) {
			php_error_docref(NULL, E_WARNING, "Parameter cannot be empty!");
		}
		return;
	}


	if (Z_TYPE_P(filename) == IS_STRING) {

		if (strncasecmp(Z_STRVAL_P(filename) + Z_STRLEN_P(filename) - 3, "ini", 3) == 0) {

			instance = alone_config_ini_instance(&zconfig, filename);
			zval_ptr_dtor(filename);
			if (!instance) {
				zval_ptr_dtor(instance);
				return;
			}

			RETURN_ZVAL(&zconfig, 1, 0);

		} else if (strncasecmp(Z_STRVAL_P(filename) + Z_STRLEN_P(filename) - 4, "yaml", 4) == 0) {

			instance = alone_config_yaml_instance(&zconfig, filename);
			zval_ptr_dtor(filename);
			if (!instance) {
				zval_ptr_dtor(instance);
				return;
			}

			RETURN_ZVAL(&zconfig, 1, 0);

		}  else {
			php_error_docref(NULL, E_WARNING, "File type mismatch，The file suffix should be .ini、.yaml、.json !");
		}

	} else {
		php_error_docref(NULL, E_WARNING, "File type mismatch，The file suffix should be .ini、.yaml、.json !");
	}

	return;
}


/** {{{ alone_config_methods
*/
zend_function_entry alone_config_methods[] = {
	PHP_ABSTRACT_ME(Alone_Config, read, NULL)
	PHP_ME(Alone_Config, factory, global_factory_arg, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC )
	{NULL, NULL, NULL}
};
/* }}} */



/** {{{ alone_STARTUP_FUNCTION
*/
ALONE_STARTUP_FUNCTION(config) {

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Alone_Config", alone_config_methods);
	alone_config_ce = zend_register_internal_class_ex(&ce, NULL);
	alone_config_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;


	zend_declare_property_null(alone_config_ce, ZEND_STRL(ALONE_CONFIG_PROPERT_NAME), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(alone_config_ce, ZEND_STRL(ALONE_CONFIG_PROPERT_NAME_READONLY), 1, ZEND_ACC_PROTECTED);

	ALONE_STARTUP(config_ini);
	ALONE_STARTUP(config_yaml);
	return SUCCESS;
}
/* }}} */

