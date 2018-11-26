#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include "Zend/zend_API.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_ini_scanner.h"


#include "php_alone.h"
#include "alone_config.h"
#include "configs/config_ini.h"

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

zend_class_entry *alone_config_ini_ce;
zval *local_filename;

ZEND_BEGIN_ARG_INFO_EX(alone_config_ini_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(alone_config_ini_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()



alone_config_t *alone_config_ini_instance(alone_config_t *this_ptr, zval *filename)  {

	if (filename && Z_TYPE_P(filename) == IS_STRING) {
		zval configs;

		if (Z_ISUNDEF_P(this_ptr)) {
			object_init_ex(this_ptr, alone_config_ini_ce);
		}

		zend_update_property(alone_config_ini_ce, this_ptr, ZEND_STRL(ALONE_CONFIG_FILENAME), filename);
		zval_ptr_dtor(filename);

		return this_ptr;
	}

	return NULL;
}




static void alone_simple_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
	switch (callback_type) {

		case ZEND_INI_PARSER_ENTRY:
			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}
			Z_TRY_ADDREF_P(arg2);
			zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), arg2);
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
		{
			zval hash, *find_hash;

			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}

			if (!(Z_STRLEN_P(arg1) > 1 && Z_STRVAL_P(arg1)[0] == '0') && is_numeric_string(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), NULL, NULL, 0) == IS_LONG) {
				zend_ulong key = (zend_ulong) zend_atol(Z_STRVAL_P(arg1), (int)Z_STRLEN_P(arg1));
				if ((find_hash = zend_hash_index_find(Z_ARRVAL_P(arr), key)) == NULL) {
					array_init(&hash);
					find_hash = zend_hash_index_update(Z_ARRVAL_P(arr), key, &hash);
				}
			} else {
				if ((find_hash = zend_hash_find(Z_ARRVAL_P(arr), Z_STR_P(arg1))) == NULL) {
					array_init(&hash);
					find_hash = zend_hash_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), &hash);
				}
			}

			if (Z_TYPE_P(find_hash) != IS_ARRAY) {
				zval_dtor(find_hash);
				array_init(find_hash);
			}

			if (!arg3 || (Z_TYPE_P(arg3) == IS_STRING && Z_STRLEN_P(arg3) == 0)) {
				Z_TRY_ADDREF_P(arg2);
				add_next_index_zval(find_hash, arg2);
			} else {
				array_set_zval_key(Z_ARRVAL_P(find_hash), arg3, arg2);
			}
		}
		break;

		case ZEND_INI_PARSER_SECTION:
			break;
	}
}

static void alone_ini_parser_cb_with_sections(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
	if (callback_type == ZEND_INI_PARSER_SECTION) {
		array_init(&ALONE_BG(active_ini_file_section));
		zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), &ALONE_BG(active_ini_file_section));
	} else if (arg2) {
		zval *active_arr;

		if (Z_TYPE(ALONE_BG(active_ini_file_section)) != IS_UNDEF) {
			active_arr = &ALONE_BG(active_ini_file_section);
		} else {
			active_arr = arr;
		}

		alone_simple_ini_parser_cb(arg1, arg2, arg3, callback_type, active_arr);
	}
}

PHP_METHOD(Alone_Config_Ini,__construct) {
	zval *self = getThis();
	php_printf("执行 alone_Config_ini::__construct");
	if (!self) {
		RETURN_FALSE;
	}
}

/**
 * read ini file,
 */
PHP_METHOD(Alone_Config_Ini,read) {
	zval *z_filename = NULL;
	zval configs;

	z_filename = zend_read_property(alone_config_ini_ce ,getThis() ,ZEND_STRL(ALONE_CONFIG_FILENAME) ,0 ,&configs TSRMLS_DC);

	if (Z_STRVAL_P(z_filename) != NULL) {

		char *filename = Z_STRVAL_P(z_filename);
		size_t filename_len = strlen(Z_STRVAL_P(z_filename));
		zend_bool process_sections = 1;
		zend_long scanner_mode = ZEND_INI_SCANNER_NORMAL;
		zend_file_handle fh;
		zend_ini_parser_cb_t ini_parser_cb;

		if (filename_len == 0) {
			php_error_docref(NULL, E_WARNING, "Filename cannot be empty!");
			RETURN_FALSE;
		}

		/* Set callback function */
		if (process_sections) {
			ZVAL_UNDEF(&ALONE_BG(active_ini_file_section));
			ini_parser_cb = (zend_ini_parser_cb_t) alone_ini_parser_cb_with_sections;
		} else {
			ini_parser_cb = (zend_ini_parser_cb_t) alone_simple_ini_parser_cb;
		}

		/* Setup filehandle */
		memset(&fh, 0, sizeof(fh));
		fh.filename = filename;
		fh.type = ZEND_HANDLE_FILENAME;

		array_init(return_value);
		zval_ptr_dtor(z_filename);
		if (zend_parse_ini_file(&fh, 0, (int)scanner_mode, ini_parser_cb, return_value) == FAILURE) {
			zval_dtor(return_value);
			RETURN_FALSE;
		}

	} else {
		php_error_docref(NULL, E_WARNING, "Filename cannot be empty!");
	}
}

zend_function_entry alone_config_ini_methods[] = {
	//PHP_ME(yaf_config_ini, __construct,	yaf_config_ini_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	/* PHP_ME(yaf_config_ini, __destruct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR) */
	PHP_ME(Alone_Config_Ini, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Alone_Config_Ini, read, NULL, ZEND_ACC_PUBLIC)
//	PHP_ME(alone_Config_ini, get, alone_config_ini_get_arginfo, ZEND_ACC_PUBLIC)
//	PHP_ME(alone_Config_ini, set, alone_config_ini_set_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};



ALONE_STARTUP_FUNCTION(config_ini) {
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Alone_Config_Ini", alone_config_ini_methods);
	alone_config_ini_ce = zend_register_internal_class_ex(&ce, alone_config_ce);
	/*
	#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
		zend_class_implements(alone_config_ini_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
	#elif PHP_VERSION_ID >= 70200
		zend_class_implements(alone_config_ini_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
	#else
		zend_class_implements(alone_config_ini_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
	#endif

	*/
	zend_declare_property_null(alone_config_ini_ce, ZEND_STRL(ALONE_CONFIG_FILENAME), ZEND_ACC_PROTECTED);
	alone_config_ini_ce->ce_flags |= ZEND_ACC_FINAL;
	return SUCCESS;
}


