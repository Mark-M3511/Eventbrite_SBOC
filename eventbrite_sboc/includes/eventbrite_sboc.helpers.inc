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
    return check_plain(variable_get('eventbrite_sboc_oauth_token', _eventbrite_sboc_oauth_token()));
 }
 
 function _eventbrite_sboc_set_oauth_token($value){
    return variable_set('eventbrite_sboc_oauth_token', $value);
 }
 
 function _eventbrite_sboc_del_oauth_token(){
    variable_del('eventbrite_sboc_oauth_token');
 }

 function _eventbrite_sboc_event_id(){    	
  return check_plain(variable_get('eventbrite_event_id', ''));
 }
 
 function _eventbrite_sboc_private_files_dir($file_uri){
   return realpath($file_uri);
 }
 
 function _eventbrite_sboc_get_timestamp($offset='now'){
   $tz = new DateTimeZone(_eventbrite_sboc_get_php_safe_tz(date_default_timezone(FALSE)));
   $dt = new DateTime($offset, $tz);
   return $dt->format('Y-m-d H:i:s');
 }
 
 function _eventbrite_sboc_get_date($offset='now'){
   $tz = new DateTimeZone(_eventbrite_sboc_get_php_safe_tz(date_default_timezone(FALSE)));
   $dt = new DateTime($offset, $tz);
   return $dt->format('Y-m-d'); 
 }
 
 function _eventbrite_sboc_get_low_date(){
   if (!isset($_SESSION['pending_list_time_span'])) {
      $_SESSION['pending_list_time_span'] = 1;
   }
   $days = -((int)$_SESSION['pending_list_time_span']);     
   $low_date = _eventbrite_sboc_get_date("$days day"); 
   return $low_date;
 } 
 
 function _eventbrite_sboc_get_date_in_tz($timestamp, $tz_origin, $tz_target=''){
   $current_time = $timestamp;
   try{
     $tz_target = (!empty($tz_target)) ? _eventbrite_sboc_get_php_safe_tz($tz_target) : _eventbrite_sboc_get_php_safe_tz(date_default_timezone(FALSE));
	 $tz_origin = _eventbrite_sboc_get_php_safe_tz($tz_origin);
     $dt = new DateTime($timestamp, new DateTimeZone($tz_origin));
     $dt->setTimezone(new DateTimeZone($tz_target));
	 $current_time = $dt->format('Y-m-d H:i:s');
   }catch(Exception $e){
      watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $current_time; 
 }
 
 function _eventbrite_sboc_get_timezone_list(){
   $tz_list = timezone_identifiers_list();
   $result = array();
   $dt = new DateTime('now');
   foreach($tz_list as $tz){
	 $dt->setTimezone(new DateTimeZone(_eventbrite_sboc_get_php_safe_tz($tz)));
     $result[] = str_replace('_', ' ', $tz). " [".$dt->format(DateTime::RFC1123). "]"; 
   }
   return $result;
 }
 
 function _eventbrite_sboc_get_timezone_by_index($index){
   $tz_list = timezone_identifiers_list();
   if ($index < 0 || $index >= count($tz_list)){
      $index = 0;
   }
   return _eventbrite_sboc_get_php_safe_tz($tz_list[$index]);
 }
 
 function _eventbrite_sboc_get_php_safe_tz($timezone){
    return str_replace(' ', '_', $timezone);
 }
 
 function _eventbrite_sboc_get_page_num(){
   $qp = drupal_get_query_parameters();
   $page_num = 0;
   if (!empty($qp) && isset($qp['page'])){
      $page_num += (int)$qp['page'];
   }
   return $page_num;
 }

/**
 * Function _eventbrite_sboc_month_num
 *   Retrieves numerical index coressponding to the order of the month in the calendar
 * @param String month_name
 *	 Name of the month
 * @return Integer result
 *	 Numerical order of the month in the calendar
 */
 function _eventbrite_sboc_month_num($month_name){
 	$result = 1;
 	$arr_mths = &drupal_static(__FUNCTION__);
	if (!isset($arr_mths)){
		$arr_mths = array(1 => 'January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September',
		'October', 'November', 'December');
	}
	$result = array_search($month_name, $arr_mths);
	
 	return $result;
 }
 
 /**
 * Function _eventbrite_sboc_select_attachment
 *   Selects the appropraite file/attachment according to the value of category
 * @param String category
 *	 Single characeter value indicating registrants category: I=Intermediate; P=Primary; J=Junior 
 * @return String attachment
 *	 Returns fully qualified path and filename
 */ 
 function _eventbrite_sboc_select_attachment($category){  
    $attachment = DRUPAL_ROOT . '/'. variable_get('file_private_path', '') . '/';
    switch($category){
      case 'I':
	    $attachment .= variable_get('eventbrite_sboc_filename_3', '');
	    break;
	  case 'J':	  
	    $attachment .= variable_get('eventbrite_sboc_filename_2', '');
	    break; 
      case 'P':	  
	    $attachment .= variable_get('eventbrite_sboc_filename_1', '');
	    break; 
	  default:
	    $attachment = ''; 
        break;	
    }  
    return $attachment;
 }
 
/**
 * Function _eventbrite_sboc_mime_type
 *   Select MIME type according to extension
 * @param String ext
 *	 File extension
 * @return String result
 *	 Returns MIME type string
 */
 function _eventbrite_sboc_mime_type($ext){
   /* http://en.wikipedia.org/wiki/Internet_media_type */
   $result = '';
   switch($ext){    
	 case 'doc':
       $result = 'application/msword';
	   break; 
	 case 'gzip':
	 case 'zip':
       $result = 'application/zip';
	   break;   
     case 'pdf':
	   $result = 'application/pdf';
	   break;	   	
 	 case 'ppt':
       $result = 'application/mspowerpoint';
	   break;  
	 case 'xls':
	 case 'xlt':
       $result = 'application/msexcel';
	   break;        	   
	 default:
       break;	 
   }
   return $result;
 }
 
 /**
 * Function _eventbrite_sboc_restrict_string_length
 *  Restrict passed in string to a specified length
 * @param String target
 *	 File extension
 * @param int length
 *	 Allowed length
 * @return String result
 *	 Returns string less than or equal to length param
 */
 function _eventbrite_sboc_restrict_string_length($target, $length){
 	$result = $target;
 	if (drupal_strlen($result) > $length){
	  	$result = drupal_substr($result, 0, $length);
	}
 	return $result;
 }