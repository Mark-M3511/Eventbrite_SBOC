<?php

function _example_form_submit($form, &$form_state) {
	_example_form_save($form, $form_state);

	global $user;

	$accounts = array($user, user_load(XX)); //you can add accounts to this array to notify

	example_notify($accounts, $user); //could also pass $form_state or other variables

}

function example_notify($accounts, $from) {

	$params['from'] = $from;

	foreach ($accounts as $account) {

	$params['account'] = $account;

	// example_mail() will be called based on the first drupal_mail() parameter.

	drupal_mail('example', 'notice', $account->mail, user_preferred_language($account), $params, $from->mail);

	 

	drupal_set_message('This form has been emailed to '. $account->name);

	}

}

function example_mail($key, &$message, $params) {

	$data['user'] = $params['from'];

	$account = $data['user']->name;

	$attachment = array(

	'filecontent' => file_get_contents(DRUPAL_ROOT . '/README.txt'),

	'filename' => 'test.txt',

	'filemime' => 'text/plain',

	);

	switch($key) {

	case 'notice':

	$langcode = $message['language']->language;

	$message['subject'] = 'example submission from' . $account;

	$message['body'][] ='<p>'. $account .' has submitted an example.</p>';

	$message['params']['attachments'][] = $attachment;

	break;

	}

}

?>