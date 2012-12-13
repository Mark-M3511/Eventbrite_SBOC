<?php 

function example_notify($accounts, $from) {

	$params['from'] = $from;

	foreach ($accounts as $account) {

	$params['account'] = $account;

	// example_mail() will be called based on the first drupal_mail() parameter.

	drupal_mail('example', 'notice', $account->mail, user_preferred_language($account), $params, $from->mail);


	drupal_set_message('This form has been emailed to '. $account->name);

}

function example_mail($key, &$message, $params) {

	$data['user'] = $params['from'];

	$account = $data['user']->name;

	module_load_include('inc', 'print_pdf', 'print_pdf.pages');

	$file_content = module_invoke('print_pdf', 'generate_path', 'example-url/');

	$attachment = array(

	'filecontent' => $file_content,

	'filename' => 'example-filename',

	'filemime' => 'application/pdf',

	);

	 

	switch($key) {

	case 'notice':

	$langcode = $message['language']->language;

	$message['subject'] = 'example submission from '. $account;

	$message['body'][] ='<p>'. $account .' has submitted an example.</p>';

	$message['params']['attachments'][] = $attachment;

	break;

	}

}

?>