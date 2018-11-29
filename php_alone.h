/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ALONE_H
#define PHP_ALONE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <SAPI.h>
#ifdef ZTS
#	include "TSRM.h"
#endif
#include <ext/standard/info.h>
#include <ext/standard/base64.h>
#include <ext/standard/basic_functions.h>
#include <ext/standard/php_var.h>
#include <ext/standard/php_smart_string.h>
#include <Zend/zend_extensions.h>
#include <Zend/zend_hash.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_smart_str.h>
#ifdef __cplusplus
} /* extern "C" */
#endif

#include <yaml.h>


#define ALONE_STRING_INI  "ini"
#define ALONE_STRING_PHP  "php"
#define ALONE_STRING_XML  "xml"

#define ALONE_CONFIG_FILENAME   "_filename"

#define alone_config_t		zval


extern zend_module_entry alone_module_entry;
#define phpext_alone_ptr &alone_module_entry





/* default depth */
#define PHP_JSON_PARSER_DEFAULT_DEPTH 512

#define PHP_ALONE_VERSION "0.1.0" /* Replace with version number for your extension */


#ifdef ZTS
#define ALONE_BG(v) ZEND_TSRMG(alone_globals_id, zend_alone_globals *, v)
#else
#define ALONE_BG(v) (alone_globals.v)
#endif


#define ALONE_STARTUP_FUNCTION(module)   	ZEND_MINIT_FUNCTION(alone_##module)
#define ALONE_RINIT_FUNCTION(module)		ZEND_RINIT_FUNCTION(alone_##module)
#define ALONE_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(alone_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define ALONE_SHUTDOWN_FUNCTION(module)  	ZEND_MSHUTDOWN_FUNCTION(alone_##module)
#define ALONE_SHUTDOWN(module)	 	    	ZEND_MODULE_SHUTDOWN_N(alone_##module)(INIT_FUNC_ARGS_PASSTHRU)


#ifdef PHP_WIN32
#	define PHP_ALONE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ALONE_API __attribute__ ((visibility("default")))
#else
#	define PHP_ALONE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/
ZEND_BEGIN_MODULE_GLOBALS(alone)
	zend_bool decode_binary;
	zend_long decode_timestamp;
	zend_bool decode_php;
	zval *timestamp_decoder;
	char         *global_library;
	/* {{{ This only effects internally */
	zend_bool  	st_compatible;
	/* }}} */
	long		forward_limit;
	HashTable	*configs;
	zval 		 modules;
	zval        *default_route;
	zval        active_ini_file_section;
	zval        *ini_wanted_section;
	uint        parsing_flag;
	int encoder_depth;
	int encode_max_depth;
ZEND_END_MODULE_GLOBALS(alone)

extern ZEND_DECLARE_MODULE_GLOBALS(alone);


/* Always refer to the globals in your function as ALONE_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define ALONE_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(alone, v)

#if defined(ZTS) && defined(COMPILE_DL_ALONE)
ZEND_TSRMLS_CACHE_EXTERN()
#endif




#endif	/* PHP_ALONE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
