<?php

/**
 * @file
 * Provides metadata for the entity.
 */

/*
 * Implements hook_entity_property_info_alter().
 */
function commerce_cardonfile_entity_property_info_alter(&$info) {
  // Commerce Order
  if (isset($info['commerce_order']) && isset($info['commerce_order']['properties'])) {
    $order_properties = &$info['commerce_order']['properties'];
    $order_properties['chargeable_cards'] = array(
      'label' => t('Chargeable Cards on File'),
      'type' => 'list<struct>',
      'description' => t('The available cards on file that can be charged.'),
      'getter callback' => 'commerce_cardonfile_order_property_get',
      'property info' => commerce_cardonfile_data_property_info_callback(),
      'computed' => TRUE,
    );
    $order_properties['chargeable_card_preferred'] = array(
      'label' => t('Preferred Chargeable Card on File'),
      'type' => 'struct',
      'description' => t('The preferred card on file that can be charged.'),
      'getter callback' => 'commerce_cardonfile_order_property_get',
      'property info' => commerce_cardonfile_data_property_info_callback(),
      'computed' => TRUE,
    );
  }
}
