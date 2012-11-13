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
      'wrap' => TRUE,
      'wrapper class' => 'EntityStructureWrapper',
      'token type' => 'commerce-cardonfile-card-data',
      'property info' => commerce_cardonfile_data_property_info_callback(),
    ),
    'commerce_cardonfile_process_card_response' => array(
      'label' => t('process card response'),
      'group' => t('Process Card on File Response'),
      'wrap' => TRUE,
      'wrapper class' => 'EntityStructureWrapper',
      'token type' => 'commerce-cardonfile-process-card-response',
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
    'access callback' => 'commerce_cardonfile_data_rules_access',
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
    'access callback' => 'commerce_cardonfile_data_rules_access',
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

  $events['commerce_cardonfile_data_disable'] = array(
    'label' => t('After disabling a card on file'),
    'group' => t('Commerce Card on File'),
    'access callback' => 'commerce_cardonfile_data_rules_access',
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
    'label' => t('After deleting a card on file database record'),
    'group' => t('Commerce Card on File'),
    'access callback' => 'commerce_cardonfile_data_rules_access',
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

/**
 * Implements hook_rules_action_info().
 */
function commerce_cardonfile_rules_action_info() {
  $actions = array();

  $actions['commerce_cardonfile_order_process_card'] = array(
    'label' => t('Process an order with a card on file'),
    'group' => t('Commerce Card on File'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'charge' => array(
        'type' => 'commerce_price',
        'label' => t('Charge'),
        'description' => t('The charge amount and currency. If not provided, then the order balance will be used.'),
        'optional' => TRUE,
      ),
      'card_data' => array(
        'type' => 'commerce_cardonfile_card_data',
        'label' => t('Card Data'),
        'description' => t('If not provided, then the order owner\'s first chargeable card provided by hook_commerce_cardonfile_order_chargeable_cards() will be used.'),
        'optional' => TRUE,
      ),
      'forced_instance_id' => array(
        'type' => 'text',
        'label' => t('Payment method instance id'),
        'description' => t('If provided and Card Data is provided, then the instance id must match that of the card data. If not provided, then the order owner\'s first chargeable card provided by hook_commerce_cardonfile_order_chargeable_cards() will be used.'),
        'options list' => 'commerce_cardonfile_rules_payment_instance_process_options_list',
        'optional' => TRUE,
      ),
    ),
    'provides' => array(
      'process_card_response' => array(
        'type' => 'commerce_cardonfile_process_card_response',
        'label' => t('Process Card Response'),
        'save' => FALSE,
      ),
    ),
    'callbacks' => array(
      'execute' => 'commerce_cardonfile_rules_action_order_process_card',
    ),
  );


  return $actions;
}

/**
 * Options list for payment method instances that provide a process callback
 */
function commerce_cardonfile_rules_payment_instance_process_options_list() {
  $options = array('' => t('- None -'));
  $implements = commerce_cardonfile_payment_method_implements('process callback');

  foreach ($implements as $method_id => $method_function) {
    $payment_method_instances = _commerce_cardonfile_payment_method_instances($method_id, TRUE);
    if (empty($payment_method_instances)) {
      continue;
    }

    foreach ($payment_method_instances as $instance_id => $payment_method) {
      list($method_id_part, $rule_name) = explode('|', $instance_id);

      $options[$instance_id] = t('@title (rule: @rule)' , array(
        '@title' => $payment_method['title'],
        '@rule' => $rule_name,
      ));
    }
  }

  return $options;
}

/**
 * Rules action callback for commerce_cardonfile_order_process_card
 */
function commerce_cardonfile_rules_action_order_process_card($order, $charge = array(), $card_data = array(), $forced_instance_id = NULL) {
  $response = commerce_cardonfile_order_process_card($order, $charge, $card_data, $forced_instance_id);

  // return provided variables
  return array('response' => $response);
}
