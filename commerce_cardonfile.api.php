<?php

/**
 * @file
 * Hooks provided by the Commerce Card On File module.
 */

// -----------------------------------------------------------------------
// Payment method callbacks

/**
 * Implements hook_commerce_payment_method_info().
 */
function my_gateway_commerce_payment_method_info() {
  $payment_methods = array();

  $payment_methods['my_gateway_xyz'] = array(
    'base' => 'my_gateway_xyz',
    'title' => t('My Gateway - Credit Card'),
    'short_title' => t('My Gateway CC'),
    'display_title' => t('Credit card'),
    'description' => t('Integrates My Gateway Method for card not present CC transactions.'),
    'cardonfile' => array(
      'create callback' => 'my_gateway_cardonfile_create',
      'update callback' => 'my_gateway_cardonfile_update',
      'delete callback' => 'my_gateway_cardonfile_delete',
      'get address callback' => 'my_gateway_cardonfile_get_address',
      'process callback' => 'my_gateway_cardonfile_process',
    ),
  );

  return $payment_methods;
}

/**
 * Implements commerce_cardonfile 'create callback'
 *
 * @param $payment_method
 *   The payment method instance definition array.
 * @param $pane_form
 *   The pane form similar to the payment pane form during checkout.
 * @param $pane_values
 *   The pane form state values similar to the payment pane form during checkout.
 * @param $billing_addressfield
 *   The billing address array with keys the same as addressfield
 * @param $card_data
 *   The stored credit card data array to be processed. The 'remote_id' is not
 *   populated.
 *
 * @return
 *   The remote id to store with the card on file data
 */
function my_gateway_cardonfile_create($payment_method, $pane_form, $pane_values, $billing_addressfield, $card_data = array()) {
  $remote_id = my_gateway_create_customer_payment_profile($payment_method, $pane_values, $billing_addressfield, $card_data);

  return $remote_id;
}

/**
 * Implements commerce_cardonfile 'update callback'
 *
 * @param $form
 *   The card on file update form array
 * @param $form_state
 *   The card on file update form's form state array
 * @param $payment_method
 *   The payment method instance definition array.
 * @param $card_data
 *   The stored credit card data array to be processed
 *
 * @return
 *   TRUE if the operation was successful
 */
function my_gateway_cardonfile_update($form, &$form_state, $payment_method, $card_data) {
  if ($form_state['values']['credit_card']['number'] != $form['credit_card']['number']['#default_value']) {
    $number = $form_state['values']['credit_card']['number'];
  }
  else {
    $number = 'XXXX' . $card_data['card_number'];
  }

  // Load the payment profile so that billTo can be updated.
  $payment_profile_xml = my_gateway_get_customer_payment_profile_request($payment_method, $card_data);
  $billto_xml_original = $payment_profile_xml->paymentProfile->billTo;

  // Set the billTo update
  $billto_array = array();
  if (!empty($form_state['values']['commerce_customer_address'])) {
    $input_addresses = reset($form_state['values']['commerce_customer_address']);
    $input_address = reset($input_addresses);
    $billto_array = my_gateway_addressfield_to_billTo($input_address);
  }
  else {
    $billto_array = (array) $billto_xml_original;
  }

  // Extract the first and last name from credit card form values.
  if (!empty($form_state['values']['credit_card']['owner'])) {
    $name_parts = explode(' ', $form_state['values']['credit_card']['owner']);
    $billto_array['firstName'] = array_shift($name_parts);
    $billto_array['firstName'] = my_gateway_billTo_process_value('firstName', $billto_array['firstName']);

    $billto_array['lastName'] = implode(' ', $name_parts);
    $billto_array['lastName'] = my_gateway_billTo_process_value('lastName', $billto_array['lastName']);
  }

  // Build the base profile update data.
  $api_request_data = my_gateway_build_update_request($number, $card_data, $billto_array);

  // Fetch the response from the API server and let Card on File know if the
  // update was successful.
  $xml_response = my_gateway_request($payment_method, 'updateCustomerPaymentProfileRequest', $api_request_data);

  return (string) $xml_response->messages->message->code == 'I00001';
}

/**
 * Implements commerce_cardonfile 'delete callback'
 *
 * @param $form
 *   The card on file delete form array
 * @param $form_state
 *   The card on file delete form's form state array
 * @param $payment_method
 *   The payment method instance definition array.
 * @param $card_data
 *   The stored credit card data array to be processed
 *
 * @return
 *   TRUE if the operation was successful
 */
function my_gateway_cardonfile_delete($form, &$form_state, $payment_method, $card_data) {
  $xml_response = my_gateway_delete_customer_payment_profile_request($payment_method, $card_data);
  $code = (string) $xml_response->messages->message->code;

  return in_array($code, array('I00001', 'E00040'));
}

/**
 * Implements commerce_cardonfile 'get address callback'
 *
 * @param $payment_method
 *   The payment method instance definition array.
 * @param $card_data
 *   The stored credit card data array to be processed
 *
 * @return
 *   An addressfield address array with the following keys:
 *    'country'
 *    'name_line'
 *    'first_name'
 *    'last_name'
 *    'organisation_name'
 *    'administrative_area'
 *    'sub_administrative_area'
 *    'locality'
 *    'dependent_locality'
 *    'postal_code'
 *    'thoroughfare'
 *    'premise'
 *    'sub_premise'
 */
function my_gateway_cardonfile_get_address($payment_method, $card_data) {
  if (empty($card_data['instance_id']) || empty($card_data['remote_id'])) {
    return array();
  }

  $gateway_profile = my_gateway_get_customer_payment_profile_request($payment_method, $card_data);
  $billTo = $gateway_profile->billTo;
  $address = my_gateway_billTo_to_addressfield_array($billTo);

  return $address;
}

/**
 * Implements commerce_cardonfile 'process callback'
 *
 * @param $payment_method
 *  The payment method instance definition array.
 * @param $card_data
 *   The stored credit card data array to be processed
 * @param $order
 *   The order object that is being processed
 * @param $charge
 *   The price array for the charge amount with keys of 'amount' and 'currency'
 *
 * @return
 *   TRUE if the transaction was successful
 */
function my_gateway_cardonfile_process($payment_method, $card_data, $order, $charge) {
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $settings = $payment_method['settings'];

  // Shoehorn these value into a form submit function which should be an API call.
  $pane_form      = NULL;
  $pane_values    = array("cardonfile" => $card_data['card_id']);

  // This form submit should only return FALSE if the transaction fails
  if (my_gateway_submit_form_submit($payment_method, $pane_form, $pane_values, $order, $charge) !== FALSE) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}


// -----------------------------------------------------------------------
// CRUD Hooks

/**
 * Card Data Insert
 *
 * @param $card_data
 *   The inserted card data.
 */
function hook_commerce_cardonfile_data_insert($card_data) {
  // perform custom operations reacting to the creation of a new card data
}

/**
 * Card Data Update
 *
 * @param $card_data
 *   The saved card data.
  * @param $card_data_original
 *   The original unchanged card data.
 */
function hook_commerce_cardonfile_data_update($card_data, $card_data_original) {
  // perform custom operations reacting to the update of an existing
  // card data
}

/**
 * Card Data Disable
 *
 * - Disabled cards have a card data status = 0.
 * - Disabled cards are removed from the remote gateway if the payment
 *   method supports deleting.
 *
 * @param $card_data
 *   The saved card data.
  * @param $card_data_original
 *   The original unchanged card data.
 */
function hook_commerce_cardonfile_data_disable($card_data, $card_data_original) {
  // perform custom operations reacting to the disabling of an existing
  // card data
}

/**
 * Card Data Delete
 *
 * @param $card_data
 *   The deleted card data.
 */
function hook_commerce_cardonfile_data_delete($card_data) {
  // perform custom operations reacting to the deletion of an existing
  // card data
}

/**
 * Lets modules prevent the deletion of a particular card data.
 *
 * Before a card can be deleted or disabled, other modules are given the chance
 * to say whether or not the action should be allowed. Modules implementing this
 * hook can check for reference data or any other reason to prevent a product
 * from being deleted and return FALSE to prevent the action.
 *
 * This is an API level hook, so implementations should not display any messages
 * to the user (although logging to the watchdog is fine).
 *
 * @param $card_data
 *   The card data to be deleted.
 *
 * @return
 *   TRUE or FALSE indicating whether or not the given card can be deleted.
 */
function hook_commerce_cardonfile_data_can_delete($card_data) {
  if (!empty($card_data['card_id'])) {
    // Use EntityFieldQuery to look for orders referencing this card and do
    // not allow the delete to occur if one exists.
    $query = new EntityFieldQuery();

    $query
      ->entityCondition('entity_type', 'commerce_order', '=')
      ->entityCondition('bundle', 'commerce_order', '=')
      ->fieldCondition('commerce_cardonfile', 'card_id', $card_data['card_id'], '=')
      ->count();

    return $query->execute() > 0 ? FALSE : TRUE;
  }
}


// -----------------------------------------------------------------------
// Order processing hooks

/**
 * Lets modules define the chargeable cards associated with an order
 *
 * The first chargeable card that passes the capability checks will be returned
 * as the order's chargeable_cardonfile entity property.  This property can
 * be used for recurring payments.
 *
 * @param $order
 *   The qualified order object. The order is ensured to have an owner user
 *   account.
 *
 * @return
 *   An array of card_ids for cards on file
 */
function hook_commerce_cardonfile_order_chargeable_cards($order) {
  $possible_cards = array();

  // Default card for payment instance
  if (!empty($order->data['payment_method'])) {
    $default_cards = commerce_cardonfile_data_load_user_default_cards($order->uid, $order->data['payment_method']);
    if (!empty($default_cards)) {
      $possible_cards = array_keys($default_cards);
    }
  }

  return $possible_cards;
}


// -----------------------------------------------------------------------
// Checkout hooks

/**
 * Lets modules alter the card on file form elements that get added to the
 * payment method form during checkout
 *
 * @param $pane_form
 *  The pane form as it would be presented to a payment module
 * @param $form
 *  The complete checkout page form
 */
function hook_commerce_cardonfile_checkout_pane_form_alter(&$pane_form, $form) {
  $pane_form['cardonfile_instance_default']['#description'] = t('Your default card will be used for automatic payments for all of your subscriptions.');
}
