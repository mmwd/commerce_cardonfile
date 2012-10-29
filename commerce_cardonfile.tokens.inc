<?php

/**
 * @file
 * Builds placeholder replacement tokens for product-related data.
 */


/**
 * Implements hook_token_info().
 */
function commerce_cardonfile_token_info() {
  $type = array(
    'name' => t('Card Data'),
    'description' => t('Tokens related to card on file data.'),
    'needs-data' =>'commerce-cardonfile-card-data',
  );

  // Tokens for card data.
  $tokens = array();
  $unsupported_properties = array(
    'rule_config', 'payment_method_instance'
  );

  $props = commerce_cardonfile_data_property_info_callback();
  $props = array_diff_key($props, array_flip($unsupported_properties));

  foreach ($props as $prop_name => $prop_info) {
    $name = str_replace('_', '-', $prop_name);
    $tokens[$name] = array(
      'name' => $name,
      'description' => $name,
    );

    if (!empty($prop_info['label'])) {
      $tokens[$name]['name'] = $prop_info['label'];
    }
    if (empty($prop_info['description'])) {
      $tokens[$name]['description'] = $tokens[$name]['name'];
    }
    if (!empty($prop_info['type'])) {
      $tokens[$name]['type'] = str_replace('_', '-', $prop_info['type']);
    }
  }

  return array(
    'types' => array('commerce-cardonfile-card-data' => $type),
    'tokens' => array('commerce-cardonfile-card-data' => $tokens),
  );
}

/**
 * Implements hook_tokens().
 */
function commerce_cardonfile_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $url_options = array('absolute' => TRUE);

  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }

  $sanitize = !empty($options['sanitize']);
  $replacements = array();

  if ($type == 'commerce-cardonfile-card-data' && !empty($data['commerce-cardonfile-card-data'])) {
    $card_data = $data['commerce-cardonfile-card-data'];
    // make a pseudo wrapper for card data
    $wrapper_info = array('property info' => commerce_cardonfile_data_property_info_callback());
    $wrapper = new EntityStructureWrapper('commerce_cardonfile_card_data', $card_data, $wrapper_info);

    foreach ($tokens as $name => $original) {
      $replacement_value = NULL;
      $property_name = str_replace('-', '_', $name);
      if (isset($wrapper->{$property_name})) {
        $property_value = $wrapper->{$property_name}->value();

        // Default values for the chained tokens handled below.
        switch ($name) {
          case 'owner':
            if (empty($card_data['uid'])) {
              $name = variable_get('anonymous', t('Anonymous'));
            }
            else {
              $name = $property_value->name;
            }
            $replacement_value = $name;
            break;

          default:
            if (!is_object($property_value)) {
              $replacement_value = $property_value;
            }
            break;
        }
      }

      if (isset($replacement_value)) {
        $replacements[$original] = $sanitize ? check_plain($replacement_value) : $replacement_value;
      }
    }

    if ($owner_tokens = token_find_with_prefix($tokens, 'owner')) {
      $owner = user_load($card_data['uid']);
      $replacements += token_generate('user', $owner_tokens, array('user' => $owner), $options);
    }

    // timestamp tokens
    foreach (array('created', 'changed', 'card_exp_timestamp') as $date) {
      if ($created_tokens = token_find_with_prefix($tokens, $date)) {
        $replacements += token_generate('date', $created_tokens, array('date' => $card_data[$date]), $options);
      }
    }
  }

  return $replacements;
}
