<?php

/**
 * @file
 * User page callbacks and forms for Commerce Card on File.
 */

/**
 * Displays the form for updating cardonfile data.
 *
 * @param $card_data
 *   The data array representing a card on file.
 * @param $owner
 *   The user object of the card owner
 */
function commerce_cardonfile_create_page($card_data, $owner) {
  // Set the title of the page.
  drupal_set_title(t('Add a credit card'));

  return drupal_get_form('commerce_cardonfile_create_form', $card_data, $owner);
}

/**
 * Builds the form for creating a card on file.
 *
 * @param $card_data
 *   The data array representing a card on file.
 */
function commerce_cardonfile_create_form($form, &$form_state, $card_data, $owner) {
  $form['card_data'] = array(
    '#type' => 'value',
    '#value' => $card_data,
  );

  $form['card_owner'] = array(
    '#type' => 'value',
    '#value' => $owner,
  );

  $payment_method = array();
  $create_callback = NULL;
  if (!empty($card_data['instance_id'])) {
    // get payment method info
    $payment_method = commerce_payment_method_instance_load($card_data['instance_id']);
    $create_callback = commerce_cardonfile_payment_method_callback($payment_method, 'create callback');
  }

  // Attach card edit form
  $card_fields = array('code' => '');
  _commerce_cardonfile_attach_card_edit_form($form, $form_state, $payment_method, $card_fields);

  // Attach billing address
  _commerce_cardonfile_attach_billing_address_edit_form($form, $form_state, $payment_method);

  // disable entire form if no update callback
  $form['#disabled'] = empty($create_callback);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add card'),
    '#suffix' => l(t('Cancel'), 'user/' . $owner->uid . '/stored-payment-methods'),
  );

  return $form;
}

/**
 * Create Form validate handler
 */
function commerce_cardonfile_create_form_validate($form, &$form_state) {
  commerce_cardonfile_update_form_validate($form, $form_state);
}

/**
 * Create Form submit handler
 */
function commerce_cardonfile_create_form_submit($form, &$form_state) {
  $values = &$form_state['values'];
  $owner = $values['card_owner'];
  $card_data = $values['card_data'];

  $instance_id = $card_data['instance_id'];
  $payment_method = commerce_payment_method_instance_load($instance_id);

  // Determine the credit card type if possible for use in later code.
  $card_type = _commerce_cardonfile_get_credit_card_type($values['credit_card']['number']);

  // Update and merge the card data with items from the form.
  $card_data = array(
    'uid' => $owner->uid,
    'instance_id' => $instance_id,
    'payment_method' => $payment_method['method_id'],
    'card_type' => $card_type,
    'card_name' => $values['credit_card']['owner'],
    'card_number' => substr($values['credit_card']['number'], -4),
    'card_exp_month' => $values['credit_card']['exp_month'],
    'card_exp_year' => $values['credit_card']['exp_year'],
    'status' => 1,
  ) + $card_data;

  // resolve billing address
  $billing_addressfield = array();
  if (!empty($values['commerce_customer_address'])) {
    $billing_addressfield_languages = reset($values['commerce_customer_address']);
    if (!empty($billing_addressfield_languages)) {
      $billing_addressfield = reset($billing_addressfield_languages);
    }
  }

  // Invoke the payment method's callback.
  $create_callback = commerce_cardonfile_payment_method_callback($payment_method, 'create callback');
  $callback_return = $create_callback($payment_method, $form, $values, $billing_addressfield, $card_data);

  if (empty($callback_return)) {
    // Display a message if we failed to communicate properly with the payment
    // gateway in the Card on File create callback.
    drupal_set_message(t('We encountered an error attempting to create your card on file. Please try again and contact us if this error persists.'), 'error');
    $form_state['rebuild'] = TRUE;
    return;
  }
  // set remote id returned
  $card_data['remote_id'] = $callback_return;

  // store card data
  commerce_cardonfile_data_save($card_data);

  drupal_set_message(t('The stored card data has been updated.'));
  $form_state['redirect'] = 'user/' . $owner->uid . '/stored-payment-methods';
}

/**
 * Displays the form for updating cardonfile data.
 *
 * @param $card_data
 *   The data array representing a card on file.
 * @param $owner
 *   The user object of the card owner
 */
function commerce_cardonfile_update_page($card_data, $owner) {
  // Set the title of the page.
  drupal_set_title(t('Update your credit card data'));
  return drupal_get_form('commerce_cardonfile_update_form', $card_data, $owner);
}

/**
 * Builds the form for updating cardonfile data.
 *
 * @param $card_data
 *   The data array representing a card on file.
 */
function commerce_cardonfile_update_form($form, &$form_state, $card_data = array(), $owner = NULL) {
  global $user;

  // store card data
  $card_data = !empty($card_data) ? $card_data : array();
  $form['card_data'] = array(
    '#type' => 'value',
    '#value' => $card_data,
  );

  $owner = !empty($owner) ? $owner : NULL;
  $form['card_owner'] = array(
    '#type' => 'value',
    '#value' => $owner,
  );

  // retrieve update_callback name
  $payment_method = array();
  $update_callback = NULL;
  $get_address_callback = NULL;
  if (!empty($card_data['instance_id'])) {
    $payment_method = commerce_payment_method_instance_load($card_data['instance_id']);
    $update_callback = commerce_cardonfile_payment_method_callback($payment_method, 'update callback');
    $get_address_callback = commerce_cardonfile_payment_method_callback($payment_method, 'get address callback');
  }

  // Card defaults
  $card_default_values = array(
    'owner' => isset($card_data['card_name']) ? $card_data['card_name'] : '',
    'number' => t('(Last 4): @number', array('@number' => $card_data['card_number'])),
    'exp_month' => $card_data['card_exp_month'],
    'exp_year' => $card_data['card_exp_year'],
  );

  // Attach card edit form
  $card_fields = array();
  _commerce_cardonfile_attach_card_edit_form($form, $form_state, $payment_method, $card_fields, $card_default_values);
  $form['credit_card']['number']['#description'] = t('Leaving this field alone will leave the current card number unchanged.');

  // Attach billing address
  $address_default_values = array();

  // get address from payment method
  if (!empty($get_address_callback)) {
    if (!empty($card_data)) {
      $address_callback_return = $get_address_callback($payment_method, $card_data);
      if (!empty($address_callback_return)) {
        $address_default_values = $address_callback_return;
      }
    }
    _commerce_cardonfile_attach_billing_address_edit_form($form, $form_state, $payment_method, $address_default_values);
  }

  // disable entire form if no update callback
  $form['#disabled'] = empty($update_callback);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update card'),
    '#suffix' => l(t('Cancel'), 'user/' . $owner->uid . '/stored-payment-methods'),
  );

  return $form;
}

/**
 * Update Form validate handler
 */
function commerce_cardonfile_update_form_validate($form, &$form_state) {
  // Load the credit card helper functions from the Payment module.
  commerce_cardonfile_load_credit_card_helpers();

  $card_form_parents = array();
  $values = &$form_state['values'];

  // Validate the credit card fields.
  $card_validate_settings = array(
    'form_parents' => array_merge($card_form_parents, array('credit_card')),
  );

  // create details array for card validate
  $card_validate_details = $values['credit_card'];

  // If card number was NOT empty on form creation
  if (!empty($form['credit_card']['number']['#default_value'])) {
    // Remove number from validation details if it contains "(Last 4)"
    if (strpos($values['credit_card']['number'], '(') !== FALSE ) {
      unset($card_validate_details['number']);
    }
  }

  // validate all card info
  if (!_commerce_cardonfile_credit_card_validate($card_validate_details, $card_validate_settings)) {
    return FALSE;
  }
}

/**
 * Update Form submit handler
 */
function commerce_cardonfile_update_form_submit($form, &$form_state) {
  $values = &$form_state['values'];
  $owner = $values['card_owner'];

  // Update the card data with items from the form.
  $card_data = $values['card_data'];
  $card_data['card_name'] = $values['credit_card']['owner'];

  // Only update the last 4 if we were given a new valid credit card number.
  if ($values['credit_card']['number'] != $form['credit_card']['number']['#default_value']) {
    $card_data['card_number'] = substr($values['credit_card']['number'], -4);
    $card_data['card_type'] = _commerce_cardonfile_get_credit_card_type($values['credit_card']['number']);
  }

  $card_data['card_exp_month'] = $values['credit_card']['exp_month'];
  $card_data['card_exp_year'] = $values['credit_card']['exp_year'];

  // Invoke the payment method's card update callback.
  if (!empty($card_data['instance_id'])) {
    $payment_method = commerce_payment_method_instance_load($card_data['instance_id']);
    $update_callback = commerce_cardonfile_payment_method_callback($payment_method, 'update callback');
    $callback_return = $update_callback($form, $form_state, $payment_method, $card_data);
    if (!$callback_return) {
      // Display a message if we failed to communicate properly with the payment
      // gateway in the Card on File update callback.
      drupal_set_message(t('We encountered an error attempting to update your card data. Please try again and contact us if this error persists.'), 'error');
      $form_state['rebuild'] = TRUE;
      return;
    }
  }

  // store the card data
  commerce_cardonfile_data_save($card_data);
  drupal_set_message(t('The stored card data has been updated.'));

  // Redirect to the payment methods tab
  $form_state['redirect'] = 'user/' . $owner->uid . '/stored-payment-methods';
}

/**
 * Adds the card edit form array with defaults to the $form
 */
function _commerce_cardonfile_attach_card_edit_form(&$form, &$form_state, $payment_method, $card_fields = array(), $card_default_values = array()) {
  // Credit Card form
  // Load the credit card helper functions from the Payment module.
  commerce_cardonfile_load_credit_card_helpers();

  if (!is_array($card_default_values) || empty($card_default_values)) {
    $card_default_values = array();
  }

  if (!is_array($card_fields) || empty($card_fields)) {
    $card_fields = array();
  }

  // ensure owner
  $card_fields += array('owner' => TRUE);

  // add card types if there is a payment setting for 'card_types'
  if (!isset($fields['type']) && isset($payment_method['settings']['card_types'])) {
    // Add the credit card types array if necessary.
    $card_types = array_diff(array_values($payment_method['settings']['card_types']), array(0));

    if (!empty($card_types)) {
      $card_fields['type'] = $card_types;
    }
  }

  // generate card form
  $form += commerce_payment_credit_card_form($card_fields, $card_default_values);
}

/**
 * Adds the billing address edit form array with defaults to the $form
 */
function _commerce_cardonfile_attach_billing_address_edit_form(&$form, &$form_state, $payment_method, $address_default_values = array()) {
  // Set defaults
  if (!is_array($address_default_values) || empty($address_default_values)) {
    $address_default_values = array();
  }

  // Get addressfield form
  $address_field_form = commerce_cardonfile_billing_address_form($form, $form_state, $address_default_values);

  // add address to form in a separate container
  if (!empty($address_field_form)) {
    $form['credit_card_billing_address'] = array(
      '#type' => 'container',
      '0' => $address_field_form,
    );
  }
}

/**
 * Builds the form for deleting cardonfile data.
 *
 * @param $card_data
 *   The data array representing a card on file.
 */
function commerce_cardonfile_delete_form($form, &$form_state, $card_data) {
  $form['card_data'] = array(
    '#type' => 'value',
    '#value' => $card_data,
  );

  if (commerce_cardonfile_data_can_delete($card_data)) {
    $form = confirm_form(
      $form,
      t('Are you sure you want to delete this card?'),
      'user/' . $card_data['uid'] . '/stored-payment-methods',
      theme('card_data_overview', array('card_data' => $card_data)) .
      '<div class="messages warning">' . t('Deleting this card cannot be undone.') . '</div>',
      t('Delete')
    );
  }
  else {
    $form['card_no_delete'] = array(
      '#markup' => theme('card_data_overview', array('card_data' => $card_data)) .
      '<div class="messages warning">' . t('This card cannot be deleted or disabled.') . '</div>',
    );
  }

  return $form;
}

/**
 * Form submit handler: delete stored card data.
 */
function commerce_cardonfile_delete_form_submit($form, &$form_state) {
  $card_data = $form_state['values']['card_data'];

  if (commerce_cardonfile_data_can_delete($card_data)) {
    // Invoke the payment method's card delete callback.
    if (!empty($card_data['instance_id'])) {
      $payment_method = commerce_payment_method_instance_load($card_data['instance_id']);
      $delete_callback = commerce_cardonfile_payment_method_callback($payment_method, 'delete callback');
      $callback_return = $delete_callback($form, $form_state, $payment_method, $card_data);
      if (!$callback_return) {
        // Display a message if we failed to communicate properly with the payment
        // gateway in the Card on File delete callback.
        drupal_set_message(t('We encountered an error attempting to delete your card data. Please try again and contact us if this error persists.'), 'error');
        $form_state['redirect'] = 'user/' . $card_data['uid'] . '/stored-payment-methods';
        return;
      }
    }

    // Disable the card data but retain the record.
    $card_data['status'] = 0;
    commerce_cardonfile_data_save($card_data);

    drupal_set_message(t('The stored card data has been deleted.'));
  }
  else {
    drupal_set_message(t('This card cannot be deleted or disabled.'), 'warning');
  }

  // Redirect to the payment methods tab if the user has other stored payment.
  $stored_cards = commerce_cardonfile_data_load_multiple($card_data['uid']);

  if (!empty($stored_cards)) {
    $form_state['redirect'] = 'user/' . $card_data['uid'] . '/stored-payment-methods';
  }
  else {
    $form_state['redirect'] = 'user/' . $card_data['uid'];
  }
}
