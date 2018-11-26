#ifndef _ALONE_CONFIG_H_
#define	_ALONE_CONFIG_H_

#define ALONE_EXTRACT_FUNC_NAME            "extract"
#define ALONE_CONFIG_PROPERT_NAME          "_config"
#define ALONE_CONFIG_PROPERT_NAME_READONLY "_readonly"

extern zend_class_entry *alone_config_ce;

ALONE_STARTUP_FUNCTION(config);

#endif
