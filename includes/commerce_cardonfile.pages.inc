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
function commerce_cardonfile_update($card_data, $owner) {
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

  // initialize parents
  if (!isset($form['#parents'])){
    $form['#parents'] = array();
  }

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

  // Credit Card form
  // Load the credit card helper functions from the Payment module.
  module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');

  $card_default_values = array(
    'owner' => isset($card_data['card_name']) ? $card_data['card_name'] : '',
    'number' => t('(Last 4): @number', array('@number' => $card_data['card_number'])),
    'exp_month' => $card_data['card_exp_month'],
    'exp_year' => $card_data['card_exp_year'],
  );

  $form += commerce_payment_credit_card_form(array('owner' => TRUE), $card_default_values);
  $form['credit_card']['number']['#description'] = t('Leaving this field alone will leave the current card number unchanged.');

  // Address form
  $address_default_values = array();

  // get address from payment method
  if (!empty($card_data)) {
    $address_callback_return = $get_address_callback($payment_method, $card_data);
    if (!empty($address_callback_return)) {
      $address_default_values = $address_callback_return;
    }
  }

  // Get addressfield form
  $address_field_form = commerce_cardonfile_billing_address_form($form, $form_state, $address_default_values);

  // add to form in a separate container
  if (!empty($address_field_form)) {
    $form['card_billing_address'] = array(
      '#type' => 'container',
      '0' => $address_field_form,
    );
  }

  // disable entire form if no update callback
  $form['#disabled'] = empty($update_callback);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update card data'),
    '#suffix' => l(t('Cancel'), 'user/' . $card_data['uid'] . '/stored-payment-methods'),
  );

  return $form;
}

/**
 * Form validate handler: update stored card data.
 */
function commerce_cardonfile_update_form_validate($form, &$form_state) {
  // Load the credit card helper functions from the Payment module.
  module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');

  // Ensure non-default values are valid numbers.
  if ($form_state['values']['credit_card']['number'] != $form['credit_card']['number']['#default_value']) {
    if (!commerce_payment_validate_credit_card_number($form_state['values']['credit_card']['number'])) {
      form_set_error('credit_card][number', t('The credit card number you entered is invalid.'));
    }
  }

  // Ensure the expiration date is not being updated to a past date.
  $exp_valid = commerce_payment_validate_credit_card_exp_date($form_state['values']['credit_card']['exp_month'], $form_state['values']['credit_card']['exp_year']);

  if ($exp_valid !== TRUE) {
    form_set_error('credit_card][exp_' . $exp_valid, t('You have specified an expired credit card.'));
  }
}

/**
 * Form submit handler: update stored card data.
 */
function commerce_cardonfile_update_form_submit($form, &$form_state) {
  // Update the card data with items from the form.
  $card_data = $form_state['values']['card_data'];
  $card_data['card_name'] = $form_state['values']['credit_card']['owner'];

  // Only update the last 4 if we were given a new valid credit card number.
  if ($form_state['values']['credit_card']['number'] != $form['credit_card']['number']['#default_value']) {
    $card_data['card_number'] = substr($form_state['values']['credit_card']['number'], -4);
  }

  $card_data['card_exp_month'] = $form_state['values']['credit_card']['exp_month'];
  $card_data['card_exp_year'] = $form_state['values']['credit_card']['exp_year'];

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

  commerce_cardonfile_data_save($card_data);

  drupal_set_message(t('The stored card data has been updated.'));

  // Redirect to the payment methods tab if the user has other stored payment.
  $stored_cards = commerce_cardonfile_data_load_multiple($card_data['uid']);

  if (!empty($stored_cards)) {
    $form_state['redirect'] = 'user/' . $card_data['uid'] . '/stored-payment-methods';
  }
  else {
    $form_state['redirect'] = 'user/' . $card_data['uid'];
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

  return confirm_form(
    $form,
    t('Are you sure you want to delete this card data?'),
    'user/' . $card_data['uid'] . '/stored-payment-methods',
    theme('card_data_overview', array('card_data' => $card_data)),
    t('Delete')
  );
}

/**
 * Form submit handler: delete stored card data.
 */
function commerce_cardonfile_delete_form_submit($form, &$form_state) {
  $card_data = $form_state['values']['card_data'];

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

  // Redirect to the payment methods tab if the user has other stored payment.
  $stored_cards = commerce_cardonfile_data_load_multiple($card_data['uid']);

  if (!empty($stored_cards)) {
    $form_state['redirect'] = 'user/' . $card_data['uid'] . '/stored-payment-methods';
  }
  else {
    $form_state['redirect'] = 'user/' . $card_data['uid'];
  }
}
