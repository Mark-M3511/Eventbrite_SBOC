<?php
/**
 * @file
 * eventbrite_sboc_helpers.inc - contains helper functions used in various parts of the main module files
 *
 */

 function _eventbrite_sboc_api_logout(){
    global $user; 
    _eventbrite_sboc_del_oauth_token();
	if ($user && ($user->uid > 0) && (user_access('administer ebsboc'))){
	   drupal_goto(EVENTBRITE_SBOC_MODULE_API_AUTH_PATH);
	}else{
	   drupal_goto('<front>');
	}
 }
  
 function _eventbrite_sboc_oauth_token(){
    $output = Eventbrite::getAccessToken(); 
	return (!empty($output) ? $output : '');
 } 

 function _eventbrite_sboc_get_oauth_token(){
    return variable_get('eventbrite_sboc_oauth_token', _eventbrite_sboc_oauth_token());
 }
 
 function _eventbrite_sboc_set_oauth_token($eventbrite_sboc_oauth_token){
    return variable_set('eventbrite_sboc_oauth_token', $eventbrite_sboc_oauth_token);
 }
 
 function _eventbrite_sboc_del_oauth_token(){
    variable_del('eventbrite_sboc_oauth_token');
 }

 function _eventbrite_sboc_event_id(){    	
  return variable_get('eventbrite_event_id', '');
 }

 function _eventbrite_sboc_api_key(){
   return  variable_get('eventbrite_api_key', '');
 }

 function _eventbrite_sboc_api_user_key(){
   return variable_get('eventbrite_api_user_key', '');
 }
 
 function _eventbrite_sboc_oauth_key(){
   return variable_get('eventbrite_oauth_key', '');
 }
 
 function _eventbrite_sboc_api_user_email(){
   return variable_get('eventbrite_sboc_api_user_email', '');
 }
 
 function _eventbrite_sboc_api_user_password(){
   return variable_get('eventbrite_sboc_api_user_password', '');
 }
 
 function _eventbrite_sboc_private_files_dir($file_uri){
   return realpath($file_uri);
 }
 
 function _eventbrite_sboc_get_timestamp($offset='now'){
   $tz = new DateTimeZone(date_default_timezone(FALSE));
   $dt = new DateTime($offset, $tz);
   $current_time = $dt->format('Y-m-d H:i:s');
   return $current_time;   
 }
 
 function _eventbrite_sboc_get_date($offset='now'){
   $tz = new DateTimeZone(date_default_timezone(FALSE));
   $dt = new DateTime($offset, $tz);
   $current_time = $dt->format('Y-m-d');
   return $current_time; 
 }

