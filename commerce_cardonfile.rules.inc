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
    'commerce_cardonfile_card_data' => array(
      'label' => t('card data'),
      'group' => t('Card on File Data'),
      'property info' => commerce_cardonfile_data_property_info_callback(),
    ),
    'commerce_cardonfile_process_card_response' => array(
      'label' => t('process card response'),
      'group' => t('Process Card on File Response'),
      'property info' => commerce_cardonfile_order_process_card_response_property_info_callback(),
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
        'type' => 'commerce_cardonfile_card_data',
        'label' => t('Card Data'),
      ),
    ),
  );

  $events['commerce_cardonfile_data_update'] = array(
    'label' => t('After updating an existing card on file'),
    'group' => t('Commerce Card on File'),
    'variables' => array(
      'card_data' => array(
        'type' => 'commerce_cardonfile_card_data',
        'label' => t('Card Data'),
      ),
      'card_data_unchanged' => array(
        'type' => 'commerce_cardonfile_card_data',
        'label' => t('Unchanged Card Data'),
      ),
    ),
  );

  $events['commerce_cardonfile_data_delete'] = array(
    'label' => t('After disabling a card on file'),
    'group' => t('Commerce Card on File'),
    'variables' => array(
      'card_data' => array(
        'type' => 'commerce_cardonfile_card_data',
        'label' => t('Card Data'),
      ),
    ),
  );

  $events['commerce_cardonfile_order_process_failed'] = array(
    'label' => t('After a failed attempt to process an order'),
    'group' => t('Commerce Card on File'),
    'access callback' => 'commerce_order_rules_access',
    'variables' => array(
      'card_data' => array(
        'type' => 'commerce_cardonfile_card_data',
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
      'response' => array(
        'type' => 'commerce_cardonfile_process_card_response',
        'label' => t('Process Card Response'),
      ),
    ),
  );

  $events['commerce_cardonfile_order_process_success'] = array(
    'label' => t('After a successful processing of an order'),
    'group' => t('Commerce Card on File'),
    'access callback' => 'commerce_order_rules_access',
    'variables' => array(
      'card_data' => array(
        'type' => 'commerce_cardonfile_card_data',
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
      'response' => array(
        'type' => 'commerce_cardonfile_process_card_response',
        'label' => t('Process Card Response'),
      ),
    ),
  );

  return $events;
}
