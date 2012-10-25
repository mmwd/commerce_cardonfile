<?php

/**
 * @file
 * Provides Rules integration
 */

/**
 * Implements hook_rules_data_info().
 */
function commerce_cardonfile_rules_data_info() {
  return array(
    'card_data' => array(
      'label' => t('card data'),
      'group' => t('Commerce Card on File'),
      'property info' => commerce_cardonfile_data_property_info_callback(),
    ),
  );
}

/**
 * Implements hook_rules_event_info().
 */
function commerce_cardonfile_rules_event_info() {
  $events = array();

  $events['commerce_cardonfile_data_insert'] = array(
    'label' => t('After saving a new card on file'),
    'group' => t('Commerce Card on File'),
    'variables' => array(
      'card_data' => array(
        'type' => 'card_data',
        'label' => t('Card Data'),
      ),
    ),
  );

  $events['commerce_cardonfile_data_update'] = array(
    'label' => t('After updating an existing card on file'),
    'group' => t('Commerce Card on File'),
    'variables' => array(
      'card_data' => array(
        'type' => 'card_data',
        'label' => t('Card Data'),
      ),
      'card_data_unchanged' => array(
        'type' => 'card_data',
        'label' => t('Unchanged Card Data'),
      ),
    ),
  );

  $events['commerce_cardonfile_data_delete'] = array(
    'label' => t('After disabling a card on file'),
    'group' => t('Commerce Card on File'),
    'variables' => array(
      'card_data' => array(
        'type' => 'card_data',
        'label' => t('Card Data'),
      ),
    ),
  );

  $events['commerce_cardonfile_order_process_failed'] = array(
    'label' => t('After a failed attempt to process an order'),
    'group' => t('Commerce Card on File'),
    'variables' => array(
      'card_data' => array(
        'type' => 'card_data',
        'label' => t('Card Data'),
      ),
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'charge' => array(
        'type' => 'commerce_price',
        'label' => t('Charge'),
      ),
    ),
  );

  $events['commerce_cardonfile_order_process_success'] = array(
    'label' => t('After a successful processing of an order'),
    'group' => t('Commerce Card on File'),
    'variables' => array(
      'card_data' => array(
        'type' => 'card_data',
        'label' => t('Card Data'),
      ),
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'charge' => array(
        'type' => 'commerce_price',
        'label' => t('Charge'),
      ),
    ),
  );

  return $events;
}
