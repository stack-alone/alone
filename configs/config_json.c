/*
 * config_json.c
 *
 *  Created on: Nov 28, 2018
 *      Author: zhaoyu <9641354@qq.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include <Zend/zend_API.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_ini_scanner.h>


#include "php_alone.h"
#include "alone_config.h"
#include "configs/config_json.h"


zend_class_entry *alone_config_json_ce;


alone_config_t *alone_config_json_instance(alone_config_t *this_ptr, zval *filename)  {

	if (filename && Z_TYPE_P(filename) == IS_STRING) {
		if (Z_ISUNDEF_P(this_ptr)) {
			object_init_ex(this_ptr, alone_config_json_ce);
		}

		zend_update_property(alone_config_json_ce, this_ptr, ZEND_STRL(ALONE_CONFIG_FILENAME), filename);
		zval_ptr_dtor(filename);

		return this_ptr;
	}

	return NULL;
}


/**
PHP_METHOD(Alone_Config_Json,__construct) {
	zval *self = getThis();
	php_printf("执行 alone_Config_json::__construct");
	if (!self) {
		RETURN_FALSE;
	}
}
/

/**
 * read ini file,
 */
PHP_METHOD(Alone_Config_Json,read) {
	zval *z_filename = NULL;
	zval configs;

	z_filename = zend_read_property(alone_config_json_ce ,getThis() ,ZEND_STRL(ALONE_CONFIG_FILENAME) ,0 ,&configs TSRMLS_DC);

	if (Z_STRVAL_P(z_filename) != NULL) {
		
		zend_bool assoc = 1; /* return JS objects as PHP objects by default */
		zend_long options = 0;
		char *filename =  Z_STRVAL_P(z_filename);
		size_t filename_len = strlen(filename);
		zend_bool use_include_path = 0;
		php_stream *stream;
		zend_long offset = 0;
		zend_long maxlen = (ssize_t) PHP_STREAM_COPY_ALL;
		zval *zcontext = NULL;
		php_stream_context *context = NULL;
		zend_string *contents;
		zval func_name;
		zval cb_args[1];
		zval cb_retval;



		stream = php_stream_open_wrapper_ex(filename, "rb",
					(use_include_path ? USE_PATH : 0) | REPORT_ERRORS,
					NULL, context);
		if (!stream) {
			RETURN_FALSE;
		}

		if (offset != 0 && php_stream_seek(stream, offset, ((offset > 0) ? SEEK_SET : SEEK_END)) < 0) {
			php_error_docref(NULL, E_WARNING, "Failed to seek to position " ZEND_LONG_FMT " in the stream", offset);
			php_stream_close(stream);
			RETURN_FALSE;
		}

		if (maxlen > INT_MAX) {
			php_error_docref(NULL, E_WARNING, "maxlen truncated from " ZEND_LONG_FMT " to %d bytes", maxlen, INT_MAX);
			maxlen = INT_MAX;
		}

		if ((contents = php_stream_copy_to_mem(stream, maxlen, 0)) != NULL) {

		} else {
			php_error_docref(NULL, E_WARNING, "Depth must be greater than zero");
			RETURN_NULL();
		}

		php_stream_close(stream);

		ZVAL_STRINGL(&func_name, "json_decode", sizeof("json_decode") - 1);

		ZVAL_STR_COPY(&cb_args[0], contents);
		ZVAL_TRUE(&cb_args[1]);

		if (call_user_function_ex(EG(function_table), NULL, &func_name, &cb_retval, 2, cb_args, 0, NULL) == SUCCESS && !Z_ISUNDEF(cb_retval)) {

			RETURN_ZVAL(&cb_retval, 1, 1);
			zval_ptr_dtor(&func_name);
			zval_ptr_dtor(&cb_retval);
			zval_ptr_dtor(&cb_args[0]);
			zval_ptr_dtor(&cb_args[1]);

		} else {
			zval_ptr_dtor(&func_name);
			zval_ptr_dtor(&cb_retval);
			zval_ptr_dtor(&cb_args[0]);
			zval_ptr_dtor(&cb_args[1]);

			php_error_docref(NULL, E_WARNING, "rebind_proc PHP callback failed");
			RETURN_FALSE;
		}

		
	
		//php_json_decode_ex(return_value, str, str_len, options, depth);

	} else {
		php_error_docref(NULL, E_WARNING, "Filename cannot be empty!");
	}
}


zend_function_entry alone_config_json_methods[] = {
	//PHP_ME(Alone_Config_Json, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Alone_Config_Json, read, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};


ALONE_STARTUP_FUNCTION(config_json) {
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Alone_Config_Json", alone_config_json_methods);
	alone_config_json_ce = zend_register_internal_class_ex(&ce, alone_config_ce);

	zend_declare_property_null(alone_config_json_ce, ZEND_STRL(ALONE_CONFIG_FILENAME), ZEND_ACC_PROTECTED);
	alone_config_json_ce->ce_flags |= ZEND_ACC_FINAL;
	return SUCCESS;
}
