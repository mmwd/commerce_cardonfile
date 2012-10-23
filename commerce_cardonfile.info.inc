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
    $order_properties['chargeable_cardonfile'] = array(
      'label' => t('Chargeable Card on File'),
      'type' => 'struct',
      'description' => t('The order owner\'s available card on file that can be charged.'),
      'getter callback' => 'commerce_cardonfile_order_property_get',
      'property info' => commerce_cardonfile_data_property_info_callback(),
      'computed' => TRUE,
    );
  }
}
