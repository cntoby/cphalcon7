
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#include "pconfig.h"
#include "config/exception.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/hash.h"
#include "kernel/fcall.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "kernel/array.h"

#include "internal/arginfo.h"

/**
 * Phalcon\Config
 *
 * Phalcon\Config is designed to simplify the access to, and the use of, configuration data within applications.
 * It provides a nested object property based user interface for accessing this configuration data within
 * application code.
 *
 *<code>
 *	$config = new Phalcon\Config(array(
 *		"database" => array(
 *			"adapter" => "Mysql",
 *			"host" => "localhost",
 *			"username" => "scott",
 *			"password" => "cheetah",
 *			"dbname" => "test_db"
 *		),
 *		"phalcon" => array(
 *			"controllersDir" => "../app/controllers/",
 *			"modelsDir" => "../app/models/",
 *			"viewsDir" => "../app/views/"
 *		)
 * ));
 *</code>
 *
 */
zend_class_entry *phalcon_config_ce;

PHP_METHOD(Phalcon_Config, __construct);
PHP_METHOD(Phalcon_Config, offsetExists);
PHP_METHOD(Phalcon_Config, get);
PHP_METHOD(Phalcon_Config, offsetGet);
PHP_METHOD(Phalcon_Config, offsetSet);
PHP_METHOD(Phalcon_Config, offsetUnset);
PHP_METHOD(Phalcon_Config, merge);
PHP_METHOD(Phalcon_Config, toArray);
PHP_METHOD(Phalcon_Config, count);
PHP_METHOD(Phalcon_Config, __wakeup);
PHP_METHOD(Phalcon_Config, __set_state);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, arrayConfig)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config_get, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, defaultValue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config_merge, 0, 0, 1)
	ZEND_ARG_INFO(0, config)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config_toarray, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config___wakeup, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_config_method_entry[] = {
	PHP_ME(Phalcon_Config, __construct, arginfo_phalcon_config___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Config, offsetExists, arginfo_arrayaccess_offsetexists, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, get, arginfo_phalcon_config_get, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, offsetGet, arginfo_arrayaccess_offsetget, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, offsetSet, arginfo_arrayaccess_offsetset, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, offsetUnset, arginfo_arrayaccess_offsetunset, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, merge, arginfo_phalcon_config_merge, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, toArray, arginfo_phalcon_config_toarray, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, count, arginfo_countable_count, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, __wakeup, arginfo_phalcon_config___wakeup, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, __set_state, arginfo___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_MALIAS(Phalcon_Config, __get, offsetGet, arginfo___get, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Phalcon_Config, __set, offsetSet, arginfo___set, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Phalcon_Config, __isset, offsetExists, arginfo___isset, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Phalcon_Config, __unset, offsetUnset, arginfo___unset, ZEND_ACC_PUBLIC)
	PHP_FE_END
};


static zend_object_handlers phalcon_config_object_handlers;

typedef struct _phalcon_config_object {
	zend_object obj;  /**< Zend object data */
	HashTable* props; /**< Properties */
} phalcon_config_object;

/**
 * @brief Counts the number of elements in the configuration; this is the part of Countable interface
 */
static int phalcon_config_count_elements(zval *object, long int *count)
{
	phalcon_config_object* obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);
	*count = zend_hash_num_elements(obj->props);
	return SUCCESS;
}

/**
 * @brief Common part of @c __get() and @c offsetGet()
 */
static zval* phalcon_config_read_internal(phalcon_config_object *object, zval *key, int type)
{
	zval *retval;

	if (UNEXPECTED(!key) || Z_TYPE_P(key) == IS_NULL) {
		return &EG(uninitialized_zval);
	}

	retval = phalcon_hash_get(object->props, key, type);
	return retval ? retval : NULL;
}

/**
 * @brief @c read_property handler, used instead of @c __get() magic method
 */
static zval* phalcon_config_read_property(zval *object, zval *offset, int type, void **cache_slot, zval *rv)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		if (BP_VAR_IS == type && !zend_get_std_object_handlers()->has_property(object, offset, 0, cache_slot, rv)) {
			return &EG(uninitialized_zval);
		}

		return zend_get_std_object_handlers()->read_property(object, offset, type, cache_slot, rv);
	}

	return phalcon_config_read_internal(obj, offset, type);
}

/**
 * @brief @c read_dimension handler, used instead of @c offsetGet() method
 */
static zval* phalcon_config_read_dimension(zval *object, zval *offset, int type, zval *rv)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		if (BP_VAR_IS == type && !zend_get_std_object_handlers()->has_dimension(object, offset, 0, rv)) {
			return &EG(uninitialized_zval);
		}

		return zend_get_std_object_handlers()->read_dimension(object, offset, type, rv);
	}

	return phalcon_config_read_internal(obj, offset, type);
}

/**
 * @brief Common part of @c __set() and @c offsetSet()
 */
static void phalcon_config_write_internal(phalcon_config_object *object, zval *offset, zval *value)
{
	if (Z_TYPE_P(value) == IS_ARRAY) {
		zval *instance;
		HashTable *h = Z_ARRVAL_P(value);

		if (!h->nApplyCount) {
			++h->nApplyCount;
			PHALCON_ALLOC_GHOST_ZVAL(instance);
			object_init_ex(instance, phalcon_config_ce);
			phalcon_config_construct_internal(instance, value);
			phalcon_hash_update_or_insert(object->props, offset, instance);
			--h->nApplyCount;
		}
		else {
			php_error_docref(NULL, E_WARNING, "Recursion detected");
		}
	}
	else {
		Z_ADDREF_P(value);
		phalcon_hash_update_or_insert(object->props, offset, value);
	}
}

/**
 * @brief @c write_property handler, used instead of @c __set() magic method
 */
static void phalcon_config_write_property(zval *object, zval *offset, zval *value, void **cache_slot)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		zend_get_std_object_handlers()->write_property(object, offset, value, cache_slot);
		return;
	}

	phalcon_config_write_internal(obj, offset, value);
}

/**
 * @brief @c write_dimension handler, used instead of @c offsetSet() method
 */
static void phalcon_config_write_dimension(zval *object, zval *offset, zval *value)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		zend_get_std_object_handlers()->write_dimension(object, offset, value);
		return;
	}

	phalcon_config_write_internal(obj, offset, value);
}

/**
 * @brief Common part of @c __isset() and @c offsetExists()
 */
static int phalcon_config_has_internal(phalcon_config_object *object, zval *key, int check_empty)
{
	zval *tmp = phalcon_hash_get(object->props, key, BP_VAR_UNSET);

	if (!tmp) {
		return 0;
	}

	if (0 == check_empty) {
		return Z_TYPE_P(tmp) != IS_NULL;
	}

	if (1 == check_empty) {
		return zend_is_true(tmp);
	}

	return 1;
}

static int phalcon_config_has_property(zval *object, zval *offset, int has_set_exists, void **cache_slot)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		return zend_get_std_object_handlers()->has_property(object, offset, has_set_exists, cache_slot);
	}

	return phalcon_config_has_internal(obj, offset, 0);
}

static int phalcon_config_has_dimension(zval *object, zval *offset, int check_empty)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		return zend_get_std_object_handlers()->has_dimension(object, offset, check_empty);
	}

	return phalcon_config_has_internal(obj, offset, check_empty);
}

/**
 * @brief Common part of @c __unset() and @c offsetUnset()
 */
static void phalcon_config_unset_internal(phalcon_config_object *obj, zval *key)
{
	phalcon_hash_unset(obj->props, key);
}

static void phalcon_config_unset_property(zval *object, zval *member, void **cache_slot)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		zend_get_std_object_handlers()->unset_property(object, member, cache_slot);
		return;
	}

	phalcon_config_unset_internal(obj, member);
}

static void phalcon_config_unset_dimension(zval *object, zval *offset)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);

	if (!is_phalcon_class(obj->obj.ce)) {
		zend_get_std_object_handlers()->unset_dimension(object, offset);
		return;
	}

	phalcon_config_unset_internal(obj, offset);
}

/**
 * @brief Returns the list of all internal properties. Used by @c print_r() and other functions
 */
static HashTable* phalcon_config_get_properties(zval* object)
{
	HashTable* props = zend_std_get_properties(object);

	if (!GC_G(gc_active)) {
		phalcon_config_object* obj = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);
		zend_hash_copy(props, obj->props, (copy_ctor_func_t)zval_add_ref);
	}

	return props;
}

/**
 * @brief Fast object compare function
 */
static int phalcon_config_compare_objects(zval *object1, zval *object2)
{
	phalcon_config_object *zobj1, *zobj2;
	zval result;

	zobj1 = PHALCON_GET_OBJECT_FROM_ZVAL(object1, phalcon_config_object);
	zobj2 = PHALCON_GET_OBJECT_FROM_ZVAL(object2, phalcon_config_object);

	if (zobj1->obj.ce != zobj2->obj.ce) {
		return 1;
	}

	if (zobj1->props == zobj2->props) {
		return 0;
	}

	zend_compare_symbol_tables(&result, zobj1->props, zobj2->props);
	assert(Z_TYPE_P(&result) == IS_LONG);
	return Z_LVAL_P(&result);
}

/**
 * @brief Frees all memory associated with @c phalcon_config_object
 */
static void phalcon_config_object_dtor(void* v)
{
	phalcon_config_object* obj = v;

	zend_hash_destroy(obj->props);
	FREE_HASHTABLE(obj->props);
	zend_object_std_dtor(&(obj->obj));
	efree(obj);
}

/**
 * @brief Constructs @c phalcon_config_object
 */
static zend_object *phalcon_config_object_ctor(zend_class_entry* ce)
{
	phalcon_config_object *obj = ecalloc(1, sizeof(phalcon_config_object));

	zend_object_std_init(&obj->obj, ce);
	object_properties_init(&obj->obj, ce);

	ALLOC_HASHTABLE(obj->props);
	zend_hash_init(obj->props, 0, NULL, ZVAL_PTR_DTOR, 0);

	phalcon_config_object_handlers.offset = XtOffsetof(phalcon_config_object, obj);
    phalcon_config_object_handlers.free_obj = phalcon_config_object_dtor;

	return &obj->obj;
}

static zend_object *phalcon_config_clone_obj(zval *zobject)
{
	phalcon_config_object *old_object;
	phalcon_config_object *new_object;

	old_object  = PHALCON_GET_OBJECT_FROM_ZVAL(object, phalcon_config_object);
	new_object = PHALCON_GET_OBJECT_FROM_OBJ(phalcon_di_ctor(Z_OBJCE_P(zobject)), phalcon_config_object);

	zend_objects_clone_members(&new_object->obj, &old_object->obj);
	zend_hash_copy(new_object->props, old_object->props, (copy_ctor_func_t)zval_add_ref);

	return &new_object->obj;
}

/**
 * Phalcon\Config initializer
 */
PHALCON_INIT_CLASS(Phalcon_Config){

	PHALCON_REGISTER_CLASS(Phalcon, Config, config, phalcon_config_method_entry, 0);

	phalcon_config_ce->create_object = phalcon_config_object_ctor;

	phalcon_config_object_handlers = *zend_get_std_object_handlers();
	phalcon_config_object_handlers.count_elements  = phalcon_config_count_elements;
	phalcon_config_object_handlers.read_property   = phalcon_config_read_property;
	phalcon_config_object_handlers.write_property  = phalcon_config_write_property;
	phalcon_config_object_handlers.unset_property  = phalcon_config_unset_property;
	phalcon_config_object_handlers.has_property    = phalcon_config_has_property;
	phalcon_config_object_handlers.read_dimension  = phalcon_config_read_dimension;
	phalcon_config_object_handlers.write_dimension = phalcon_config_write_dimension;
	phalcon_config_object_handlers.unset_dimension = phalcon_config_unset_dimension;
	phalcon_config_object_handlers.has_dimension   = phalcon_config_has_dimension;
	phalcon_config_object_handlers.get_properties  = phalcon_config_get_properties;
	phalcon_config_object_handlers.compare_objects = phalcon_config_compare_objects;
	phalcon_config_object_handlers.clone_obj       = phalcon_config_clone_obj;

	zend_class_implements(phalcon_config_ce, 2, zend_ce_arrayaccess, spl_ce_Countable);

	return SUCCESS;
}

/**
 * Internal implementation of __construct(). Used to avoid calls to userspace when
 * recursively walking the configuration array. Does not use MM.
 */
void phalcon_config_construct_internal(zval* this_ptr, zval *array_config)
{
	zval *pzval;
	zend_string *key;
	ulong idx;
	phalcon_config_object* obj;

	if (!array_config || Z_TYPE_P(array_config) == IS_NULL) {
		return;
	}

	obj = PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array_config), idx, key, pzval) {
		zval tmp;
		if (key) {
			ZVAL_STR(&tmp, key);
		} else {
			ZVAL_LONG(&tmp, idx);
		}
		phalcon_config_write_internal(obj, &tmp, pzval);
	} ZEND_HASH_FOREACH_END();
}

/**
 * Internal implementation of non-recursive @c toArray(). Used as an alternative
 * to @c get_object_properties().
 */
PHALCON_ATTR_WARN_UNUSED_RESULT static int phalcon_config_toarray_internal(zval *return_value, zval *this_ptr)
{
	phalcon_config_object *obj = PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object);
	int result;

	assert(!EG(exception));
	if (likely(obj->obj.ce == phalcon_config_ce)) {
		array_init_size(return_value, zend_hash_num_elements(obj->props));
		zend_hash_copy(Z_ARRVAL_P(return_value), obj->props, (copy_ctor_func_t)zval_add_ref);
		return SUCCESS;
	}

	if (phalcon_method_exists_ex(getThis(), SS("toarray")) == SUCCESS) {
		result = phalcon_return_call_method(&return_value, getThis(), "toarray", 0, NULL);
	}
	else {
		zval *params[] = { this_ptr };
		result = phalcon_call_func_aparams(&return_value, SL("get_object_vars"), 1, params);
	}

	return result;
}

/**
 * Phalcon\Config constructor
 *
 * @param array $arrayConfig
 */
PHP_METHOD(Phalcon_Config, __construct){

	zval *array_config = NULL;

	phalcon_fetch_params(0, 0, 1, &array_config);
	
	/** 
	 * Throw exceptions if bad parameters are passed
	 */
	if (array_config && Z_TYPE_P(array_config) != IS_ARRAY && Z_TYPE_P(array_config) != IS_NULL) {
		PHALCON_THROW_EXCEPTION_STRW(phalcon_config_exception_ce, "The configuration must be an Array");
		return;
	}

	phalcon_config_construct_internal(getThis(), array_config);
}

/**
 * Allows to check whether an attribute is defined using the array-syntax
 *
 *<code>
 * var_dump(isset($config['database']));
 *</code>
 *
 * @param string $index
 * @return boolean
 */
PHP_METHOD(Phalcon_Config, offsetExists){

	zval *index;

	phalcon_fetch_params(0, 1, 0, &index);
	RETURN_BOOL(phalcon_config_has_internal(PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object), index, 0));
}

/**
 * Gets an attribute from the configuration, if the attribute isn't defined returns null
 * If the value is exactly null or is not defined the default value will be used instead
 *
 *<code>
 * echo $config->get('controllersDir', '../app/controllers/');
 *</code>
 *
 * @param string $index
 * @param mixed $defaultValue
 * @return mixed
 */
PHP_METHOD(Phalcon_Config, get){

	zval *index, *default_value = NULL, *value;

	phalcon_fetch_params(0, 1, 1, &index, &default_value);

	value = phalcon_config_read_internal(PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object), index, BP_VAR_UNSET);
	if (!value || Z_TYPE_P(value) == IS_NULL) {
		if (default_value) {
			RETURN_ZVAL(default_value, 1, 0);
		}

		RETURN_NULL();
	}

	RETURN_ZVAL(value, 1, 0);
}

/**
 * Gets an attribute using the array-syntax
 *
 *<code>
 * print_r($config['database']);
 *</code>
 *
 * @param string $index
 * @return string
 */
PHP_METHOD(Phalcon_Config, offsetGet){

	zval *index;
	zval* retval;

	phalcon_fetch_params(0, 1, 0, &index);

	retval = phalcon_config_read_internal(PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object), index, BP_VAR_R);
	RETURN_ZVAL(retval, 1, 0);
}

/**
 * Sets an attribute using the array-syntax
 *
 *<code>
 * $config['database'] = array('type' => 'Sqlite');
 *</code>
 *
 * @param string $index
 * @param mixed $value
 */
PHP_METHOD(Phalcon_Config, offsetSet){

	zval *index, *value;

	phalcon_fetch_params(0, 2, 0, &index, &value);
	phalcon_config_write_internal(PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object), index, value);
}

/**
 * Unsets an attribute using the array-syntax
 *
 *<code>
 * unset($config['database']);
 *</code>
 *
 * @param string $index
 */
PHP_METHOD(Phalcon_Config, offsetUnset){

	zval *index;

	phalcon_fetch_params(0, 1, 0, &index);
	phalcon_config_unset_internal(PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object), index);
}

/**
 * Merges a configuration into the current one
 *
 * @brief void Phalcon\Config::merge(array|object $with)
 *
 *<code>
 *	$appConfig = new Phalcon\Config(array('database' => array('host' => 'localhost')));
 *	$globalConfig->merge($config2);
 *</code>
 *
 * @param Phalcon\Config $config
 * @return Phalcon\Config
 */
PHP_METHOD(Phalcon_Config, merge){

	zval *config, *array_config = NULL, *pzval, *active_value = NULL;
	zend_string *key;
	ulong idx;
	phalcon_config_object *obj;

	phalcon_fetch_params(0, 1, 0, &config);
	
	if (Z_TYPE_P(config) != IS_OBJECT && Z_TYPE_P(config) != IS_ARRAY) {
		zend_throw_exception_ex(phalcon_config_exception_ce, 0, "Configuration must be an object or array");
		return;
	}

	if (Z_TYPE_P(config) == IS_OBJECT) {
		ALLOC_INIT_ZVAL(array_config);
		if (FAILURE == phalcon_config_toarray_internal(&array_config, config)) {
			zval_ptr_dtor(array_config);
			return;
		}
	} else {
		array_config = config;
		Z_ADDREF_P(array_config);
	};

	obj = PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array_config), idx, key, pzval) {
		zval tmp;
		if (key) {
			ZVAL_STR(&tmp, key);
		} else {
			ZVAL_LONG(&tmp, idx);
		}
		active_value = phalcon_config_read_internal(obj, &tmp, BP_VAR_UNSET);

		/**
		 * The key is already defined in the object, we have to merge it
		 */
		if (active_value) {
			if ((Z_TYPE_P(pzval)  == IS_OBJECT || Z_TYPE_P(pzval) == IS_ARRAY) && Z_TYPE_P(active_value) == IS_OBJECT) {
				if (phalcon_method_exists_ex(active_value, SS("merge")) == SUCCESS) { /* Path AAA in the test */
					zval *params[] = { pzval };
					if (FAILURE == phalcon_call_method(NULL, active_value, "merge", 1, params)) {
						break;
					}
				} else { /* Path AAB in the test */
					phalcon_config_write_internal(obj, tmp, pzval);
				}
			} else { /* Path AE in the test */
				phalcon_config_write_internal(obj, tmp, pzval);
			}
		} else { /* Path B in the test */
			/**
			 * The key is not defined in the object, add it
			 */
			phalcon_config_write_internal(obj, tmp, pzval);
		}
	} ZEND_HASH_FOREACH_END();

	zval_ptr_dtor(array_config);

	RETURN_THISW();
}

/**
 * Converts recursively the object to an array
 *
 * @brief array Phalcon\Config::toArray(bool $recursive = true);
 *
 *<code>
 *	print_r($config->toArray());
 *</code>
 *
 * @return array
 */
PHP_METHOD(Phalcon_Config, toArray){

	zval *recursive = NULL, *pzval;
	zend_string *key;
	ulong idx;
	phalcon_config_object *obj;

	phalcon_fetch_params(0, 0, 1, &recursive);

	obj = PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object);
	array_init_size(return_value, zend_hash_num_elements(obj->props));
	zend_hash_copy(Z_ARRVAL_P(return_value), obj->props, (copy_ctor_func_t)zval_add_ref);

	if (!recursive || zend_is_true(recursive)) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(return_value), idx, key, pzval) {
			zval tmp;
			if (key) {
				ZVAL_STR(&tmp, key);
			} else {
				ZVAL_LONG(&tmp, idx);
			}
			if (Z_TYPE_P(pzval) == IS_OBJECT && phalcon_method_exists_ex(pzval, SS("toarray")) == SUCCESS) {
				zval *array_value = NULL;
				if (FAILURE == phalcon_call_method(&array_value, pzval, "toarray", 0, NULL)) {
					break;
				}

				phalcon_array_update_zval(return_value, &tmp, array_value, 0);
			}
		} ZEND_HASH_FOREACH_END();
	}
}

PHP_METHOD(Phalcon_Config, count)
{
	long int cnt;

	phalcon_config_count_elements(getThis(), &cnt);
	RETURN_LONG(cnt);
}

PHP_METHOD(Phalcon_Config, __wakeup)
{
	HashTable *props;
	phalcon_config_object *obj;

	obj   = PHALCON_GET_OBJECT_FROM_ZVAL(getThis(), phalcon_config_object);
	props = zend_std_get_properties(getThis());
	zend_hash_copy(obj->props, props, (copy_ctor_func_t)zval_add_ref);
}

/**
 * Restores the state of a Phalcon\Config object
 *
 * @param array $data
 * @return Phalcon\Config
 */
PHP_METHOD(Phalcon_Config, __set_state){

	zval *data;

	phalcon_fetch_params(0, 1, 0, &data);

	object_init_ex(return_value, phalcon_config_ce);
	phalcon_config_construct_internal(return_value, data);
}
