#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include "Zend/zend_interfaces.h"

#include "php_alone.h"
#include "alone_config.h"
#include "include/yaml/php_yaml_int.h"
#include "configs/config_yaml.h"


zend_class_entry *alone_config_yaml_ce;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

alone_config_t *alone_config_yaml_instance(alone_config_t *this_ptr, zval *filename)  {

	if (filename && Z_TYPE_P(filename) == IS_STRING) {

		if (Z_ISUNDEF_P(this_ptr)) {
			object_init_ex(this_ptr, alone_config_yaml_ce);
		}

		zend_update_property(alone_config_yaml_ce, this_ptr, ZEND_STRL(ALONE_CONFIG_FILENAME), filename);
		zval_ptr_dtor(filename);

		return this_ptr;
	}

	return NULL;
}
/**
PHP_METHOD(Alone_Config_Yaml,__construct) {
	 php_printf("执行alone_Config_yaml::__construct");
}
*/

PHP_METHOD(Alone_Config_Yaml,read) {

	char *filename = { 0 };
	size_t filename_len = 0;
	zend_long pos = 0;
	zval *zndocs = { 0 };
	zval configs;
	zval *z_filename = NULL;
	zval *zcallbacks = { 0 };

	php_stream *stream = { 0 };
	FILE *fp = { 0 };

	parser_state_t state;
	zval yaml;
	zend_long ndocs = 0;

	memset(&state, 0, sizeof(state));
	state.have_event = 0;
	state.callbacks = NULL;

	ALONE_BG(timestamp_decoder) = NULL;

	z_filename = zend_read_property(alone_config_yaml_ce ,getThis() ,ZEND_STRL(ALONE_CONFIG_FILENAME) ,0 ,&configs TSRMLS_DC);
	filename = Z_STRVAL_P(z_filename);
	filename_len = strlen(filename);
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
					"|lza/", &pos, &zndocs,
					&zcallbacks)) {
		return;
	}

	if (zcallbacks != NULL) {
		state.callbacks = Z_ARRVAL_P(zcallbacks);
		if (FAILURE == php_yaml_check_callbacks(state.callbacks TSRMLS_CC)) {
			RETURN_FALSE;
		}

		state.eval_func = eval_scalar_with_callbacks;

	} else {
		state.eval_func = eval_scalar;
	}

	if (NULL == (stream = php_stream_open_wrapper(filename, "rb",
			IGNORE_URL | REPORT_ERRORS | STREAM_WILL_CAST,
			NULL))) {
		RETURN_FALSE;
	}

	if (FAILURE == php_stream_cast(
			stream, PHP_STREAM_AS_STDIO, (void **) &fp, 1)) {
		php_stream_close(stream);
		RETURN_FALSE;
	}

	yaml_parser_initialize(&state.parser);
	yaml_parser_set_input_file(&state.parser, fp);

	if (pos < 0) {
		php_yaml_read_all(&state, &ndocs, &yaml TSRMLS_CC);

	} else {
		php_yaml_read_partial(&state, pos, &ndocs, &yaml TSRMLS_CC);
	}

	yaml_parser_delete(&state.parser);
	php_stream_close(stream);

	if (zndocs != NULL) {
		/* copy document count to var user sent in */
		zval_dtor(zndocs);
		ZVAL_LONG(zndocs, ndocs);
	}

	if (Z_TYPE_P(&yaml) == IS_UNDEF) {
		RETURN_FALSE;
	}

	RETURN_ZVAL(&yaml, 1, 1);
}


zend_function_entry alone_config_yaml_methods[] = {
	//PHP_ME(yaf_config_ilni, __construct,	yaf_config_ini_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	/* PHP_ME(yaf_config_ini, __destruct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR) */
	//PHP_ME(Alone_Config_Yaml, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Alone_Config_Yaml, read, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};



ALONE_STARTUP_FUNCTION(config_yaml) {
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Alone_Config_Yaml", alone_config_yaml_methods);
	alone_config_yaml_ce = zend_register_internal_class_ex(&ce, alone_config_ce);

//	#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
//		zend_class_implements(alone_config_yaml_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
//	#elif PHP_VERSION_ID >= 70200
//		zend_class_implements(alone_config_yaml_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
//	#else
//		zend_class_implements(alone_config_yaml_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
//	#endif
	zend_declare_property_null(alone_config_yaml_ce, ZEND_STRL(ALONE_CONFIG_FILENAME), ZEND_ACC_PROTECTED);
	alone_config_yaml_ce->ce_flags |= ZEND_ACC_FINAL;
	return SUCCESS;
}
