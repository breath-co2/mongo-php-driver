/**
 *  Copyright 2009-2013 10gen, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <php.h>
#include "../php_mongo.h"
#include "code.h"

zend_class_entry *mongo_ce_Code = NULL;

/* {{{ MongoCode::__construct(string code[, array scope])
 * Creates a new MongoCode object
 */
PHP_METHOD(MongoCode, __construct)
{
	char *code;
	int code_len;
	zval *zcope = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a/", &code, &code_len, &zcope) == FAILURE) {
		return;
	}

	php_mongocode_populate(getThis(), code, code_len, zcope TSRMLS_CC);
}
/* }}} */

int php_mongocode_populate(zval *mongocode_object, char *code, int code_len, zval *scope TSRMLS_DC)
{
	zend_update_property_stringl(mongo_ce_Code, mongocode_object, "code", strlen("code"), code, code_len TSRMLS_CC);

	if (scope) {
		if (Z_TYPE_P(scope) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'scope' argument of MongoCode is expected to be array, %s given", zend_get_type_by_const(Z_TYPE_P(scope)));
			return 0;
		}

		zend_update_property(mongo_ce_Code, mongocode_object, "scope", strlen("scope"), scope TSRMLS_CC);
	} else {
		/* The bson serialization requires this prop to be array */
		zval *tmp;

		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		zend_update_property(mongo_ce_Code, mongocode_object, "scope", strlen("scope"), tmp TSRMLS_CC);
		zval_ptr_dtor(&tmp);
	}

	return 1;
}

/* {{{ MongoCode::__toString()
 */
PHP_METHOD(MongoCode, __toString)
{
	zval *zode = zend_read_property(mongo_ce_Code, getThis(), "code", strlen("code"), NOISY TSRMLS_CC);
	convert_to_string_ex(&zode);

	RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */


static zend_function_entry MongoCode_methods[] = {
	PHP_ME(MongoCode, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCode, __toString, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void mongo_init_MongoCode(TSRMLS_D)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "MongoCode", MongoCode_methods);
	ce.create_object = php_mongo_type_object_new;
	mongo_ce_Code = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_string(mongo_ce_Code, "code", strlen("code"), "", ZEND_ACC_PUBLIC|MONGO_ACC_READ_ONLY TSRMLS_CC);
	zend_declare_property_null(mongo_ce_Code, "scope", strlen("scope"), ZEND_ACC_PUBLIC|MONGO_ACC_READ_ONLY  TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
