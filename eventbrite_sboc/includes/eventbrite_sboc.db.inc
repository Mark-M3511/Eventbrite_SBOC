<?php
	
 /**
 * @Database
 * Function _eventbrite_sboc_get_attendees_from_db
 *   Retrieves attendee records from database
 * @param
 *	 None
 * @return Array result
 *	 Array of dataset rows
 */
 function _eventbrite_sboc_get_attendees_from_db(){  
   $result = NULL;	
   $low_date = _eventbrite_sboc_get_low_date();
   $upper_limit = variable_get('eventbrite_sboc_email_send_limit', EVENTBRITE_SBOC_EMAIL_SEND_LIMIT);
   $event_id = _eventbrite_sboc_event_id();
   $where = "(e.event_id = :event_id)";
   $where .= " AND ";
   $where .= "(DATE_FORMAT(e.create_date,'%Y-%m-%d') >= :low_date')";
   $where_params =	array(':event_id' => $event_id, ':low_date' => $start_date,);
   try {
   		$query = db_select('eventbrite_sboc_attendees', 'e')  
	   		->fields('e', array('event_id', 'attendee_id', 'order_id', 'email_address', 'email_sent', 'email_send_date',))   
	   		->where($where, $where_params)
   			->orderBy('attendee_id', variable_get('eventbrite_sboc_app_data_sort_order', EVENTBRITE_SBOC_LIST_SORT_ORDER))
	   		 ->range(0, $upper_limit * 2);
	   		_eventbrite_sboc_debug('_eventbrite_sboc_display_sql', array('query' => $query));
   		$result = $query->execute()->fetchAll();
   }catch (Exception $e){		
		watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);   		
   }
   return $result;
 }  
 
 
 /**
 * @Database 
 * Function _eventbrite_sboc_get_attendees_in_db
 *   Retrieves attendee records from database
 * @param Array attendees
 *	 Aray of attendee records
 * @return Array result
 *	 Array of dataset rows
 */
 function _eventbrite_sboc_get_attendees_in_db($attendees){  
   $upper_limit = variable_get('eventbrite_sboc_email_send_limit', EVENTBRITE_SBOC_EMAIL_SEND_LIMIT);
   $event_id = _eventbrite_sboc_event_id();
   $rows = $result = $attendee_ids = array();
   try {
        foreach($attendees as $a){
           $attendee_ids[] = $a->attendee->id;
        }
        if (empty($attendee_ids)){
        	$attendee_ids = array(0);
        }
        $order_by = variable_get('eventbrite_sboc_app_data_sort_order', EVENTBRITE_SBOC_LIST_SORT_ORDER);
   		$query = db_select('eventbrite_sboc_attendees', 'e')  
	   		->fields('e', array('event_id', 'attendee_id', 'create_date', 'change_date', 'order_id', 
	   			'email_address', 'email_sent', 'email_send_date', 'category', 'additional_info',))  
	   		->condition('e.attendee_id', $attendee_ids, 'IN') 
   			->orderBy('attendee_id', $order_by);
// 	   		->range(0, $upper_limit * 2);
	   		_eventbrite_sboc_debug('_eventbrite_sboc_display_sql', array('query' => $query));
		$rows = $query->execute()->fetchAll();
   		foreach($rows as $row){
   		   $result[$row->attendee_id] = array(
   		   	'event_id' => $row->event_id,
   		   	'attendee_id ' => $row->attendee_id,
   		   	'create_date' => $row->create_date,
   		   	'change_date' => $row->change_date,
   		   	'order_id' => $row->order_id,
   		   	'email_address' => $row->email_address,
   		   	'email_sent' => $row->email_sent,
   		   	'email_send_date' => $row->email_send_date,
   		   	'category' => $row->category,
   		   	'additional_info' => $row->additional_info, );
   		}   		
   }catch (Exception $e){		
		watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);   		
   }
   return $result;
 }  
 
 /**
 * @Database
 * Function _eventbrite_sboc_get_attendees_from_db_paged
 *   Retreives order set of attendee records from db and configures result set for paginated tabular display
 * @param String start_date
 *	 Minimum date value
  * @param String end_date
 *	 Maximum date value
 * @return Array result
 *	 Array of dataset rows
 */
 function _eventbrite_sboc_get_attendees_from_db_paged($start_date, $end_date, $header){ 
   $result = NULL; 
   $start_date = check_plain($start_date);
   $end_date = check_plain($end_date);   
   $event_id = _eventbrite_sboc_event_id();
   $where = "(e.event_id = :event_id)";	
   $where .= ' AND ';
   $where .= "(DATE_FORMAT(e.create_date,'%Y-%m-%d') BETWEEN :start_date  AND :end_date)";
   $where_params =	array(':event_id' => $event_id, ':start_date' => $start_date, ':end_date' => $end_date,);
   try {
   		$start_date = date('Y-m-d', strtotime($start_date));
	    $end_date = date('Y-m-d', strtotime($end_date));
   		$query = db_select('eventbrite_sboc_attendees', 'e')  
   			->fields('e', array('order_id', 'create_date', 'last_name', 'first_name', 
   			   			'email_address', 'category', 'email_sent', 'email_send_date', 'change_date',))
   			->where($where, $where_params)
  		 	->extend('PagerDefault')
   			->limit(10)  
   			->extend('TableSort')
   			->orderByHeader($header)
//    			->orderBy('attendee_id', variable_get('eventbrite_sboc_app_data_sort_order', EVENTBRITE_SBOC_LIST_SORT_ORDER));
		  ->orderBy('change_date', 'DESC');
   		_eventbrite_sboc_debug('_eventbrite_sboc_display_sql', array('query' => $query));
   		$result = $query->execute()->fetchAll();  
   }catch (Exception $e){		
		watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);   		
   }		 
   return $result;
 } 
 
 /**
 * @Database
 * Function _eventbrite_sboc_get_attendees_from_db_paged_count
 *   Returns count of records for paginated display
 * @param String start_date
 *	 Minimum date value
 * @param String end date
 *	 Maximum date value
 * @return
 *	 None
 */
 function _eventbrite_sboc_get_attendees_from_db_paged_count($start_date, $end_date){  
   $row_count = 0;
   $start_date = check_plain($start_date);
   $end_date = check_plain($end_date);
   $event_id = _eventbrite_sboc_event_id();
   $where = "(e.event_id = :event_id)";	
   $where .= ' AND ';
   $where .= "(DATE_FORMAT(e.create_date,'%Y-%m-%d') BETWEEN :start_date  AND :end_date)";
   $where_params =	array(':event_id' => $event_id, ':start_date' => $start_date, ':end_date' => $end_date,);
   try {
   		$start_date = date('Y-m-d',strtotime($start_date));
	    $end_date = date('Y-m-d',strtotime($end_date));
   		$result = db_select('eventbrite_sboc_attendees', 'e')  
   			->where($where, $where_params)
   			->fields('e')   
   			->orderBy('attendee_id', variable_get('eventbrite_sboc_app_data_sort_order', EVENTBRITE_SBOC_LIST_SORT_ORDER))
   			->execute();   
   			$row_count = $result->rowCount();
   }catch (Exception $e){		
		watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);   		
   }
   return $row_count;
 } 

/**
 * @Database 
 * Function _eventbrite_sboc_select_max_event_attendee_id
 *   Retreive highest attendee record id for current event
 * @return Integer result
 *	 Attendee id
 */
 function _eventbrite_sboc_select_max_event_attendee_id(){
   $result = 0; 
   try {
   		$event_id = _eventbrite_sboc_event_id();
   		$query =  db_select('eventbrite_sboc_attendees','ea')
   					->fields('ea', 'event_id')
   					->addExpression('MAX(attendee_id)', 'max_attendee_id')
   					->condition('ea.event_id', $event_id, '=')
   					->groupBy('event_id')
   					->execute();
   }catch(Exception $e){
   	  watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
   }					
   return $result;
 }

 /**
 * @Database 
 * Function _eventbrite_sboc_search_saved_attendees
 *   Returns count of records for paginated display
 * @param Array answers
 *	 Array of customized questions and responses for each attendee record
 * @param Integer question_id
 *	 Eventbrite customized question id
 * @return Integer result
 *	 Integer indicating position of response in array
 */ 
 function _eventbrite_sboc_search_saved_attendees($attendees, $search_id){
    $result = FALSE;
	$attendee_id = check_plain($search_id);
	$event_id = _eventbrite_sboc_event_id();
	try {
   		$db_result = db_select('eventbrite_sboc_attendees', 'e')  
   			->fields('e')
   			->condition('e.attendee_id', $attendee_id, '=')   
   			->condition('e.event_id', $event_id, '=')   
   			->execute();   
   			$row_count = $db_result->rowCount();
   			$result = ($row_count > 0);
    }catch (Exception $e){		
		watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);   		
    }
    return $result; 	
 }
 
 /**
 * @Database
 * Function _eventbrite_sboc_cache_attendee_records_to_db
 *   Sends an email to each attendee
 * @param Boolean reset
 *	 Flag indicating whether session variable should be set to an empty array
 * @param Object a
 *	 Attendee record to be cached
 * @return 
 *	 None
 */
 function _eventbrite_sboc_cache_attendee_records_to_db($reset = FALSE, $a = NULL){
    
    if ($reset){
       $_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB] = array();
    }
    
    if (!$a){
       return;
    }
    
 	// $timestamp = _eventbrite_sboc_get_timestamp();
// 	$a->email_sent = 1;
// 	$a->email_send_date = $timestamp;
	$_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB][] = array(
		'event_id' => $a->event_id,
		'attendee_id' => $a->id,
		'create_date' => $a->created,
		'change_date' => $a->changed,
		'order_id' => $a->order_id,
		'email_address' => $a->email,
		'last_name' => $a->last_name,
		'first_name' => $a->first_name,
		'category' => $a->category, 
		'order_type' => $a->order_type, 
		'email_sent' => $a->email_sent, 
		'email_send_date' => $a->email_send_date,
		'reg_type' =>  $a->registration_type,
	 	'region_name' => $a->region,
	 	'gender' => $a->gender,	 		
	 	'school' => $a->school,	 		
	 	'grade' => $a->grade,
	 	'year_of_birth' => $a->yob,
	 	'month_of_birth' => $a->mob,
	 	'day_of_birth' => $a->dob,	 		
	 	'home_address_line_1' => $a->address_line_1,
	 	'home_address_line_2' => $a->address_line_2,
	 	'home_city' => $a->address_city,
	 	'home_prov_state' => $a->address_prov_state,
	 	'home_postal_zip' => $a->address_postal_zip,
	 	'home_phone_1' => $a->phone_1,
	 	'home_phone_2' => $a->phone_2,
	 	'additional_info' => $a->additional_info,
	 	'contestant_last_name' => $a->child_last_name,
	 	'contestant_first_name' => $a->child_first_name,
	 	'email_consent' => (drupal_strtoupper($a->email_consent) == 'YES' ? 1 : 0),
	 	'ticket_id' => $a->ticket_id,
	);  
 }
 
 
 /**
 * @Database 
 * Function _eventbrite_sboc_message_body
 *   Updates attendee records with changed information
 * @param Array rows
 *	 Array of attendee records
 * @return 
 *	 None
 */
 function _eventbrite_sboc_update_attendees_in_db($rows){
 
 	$event_id = _eventbrite_sboc_event_id();   
	try {	
		foreach($rows as $row){
			$query = db_update('eventbrite_sboc_attendees')
		  		->fields(array(
		  		'change_date' => $row['change_date'],
		        'email_address' => $row['email_address'],		        
	  			'last_name' => $row['last_name'], 
	  			'first_name' => $row['first_name'], 
	  			'category' => $row['category'], 
	  			'order_type' => $row['order_type'],
	  			'email_sent' => $row['email_sent'], 
	  			'email_send_date' => $row['email_send_date'],
	  			'reg_type' => $row['reg_type'],
	  			'region_name' => $row['region_name'],
	  			'gender' => $row['gender'],
	 			'school' => $row['school'],	 		
	 			'grade' => $row['grade'],
	 			'year_of_birth' => $row['year_of_birth'],
	 			'month_of_birth' => $row['month_of_birth'],
	 			'day_of_birth' => $row['day_of_birth'],	 		
	 			'home_address_line_1' => $row['home_address_line_1'],
	 			'home_address_line_2' => $row['home_address_line_2'],
	 			'home_city' => $row['home_city'],
	 			'home_prov_state' => $row['home_prov_state'],
	 			'home_postal_zip' => $row['home_postal_zip'],
	 			'home_phone_1' => $row['home_phone_1'],
	 			'home_phone_2' => $row['home_phone_2'],
	 			'additional_info' => $row['additional_info'],
	 			'contestant_last_name' => $row['contestant_last_name'],
	 			'contestant_first_name' => $row['contestant_first_name'],
	 			'email_consent' => $row['email_consent'],
	 			'ticket_id' => $row['ticket_id'],
	 			));	  
	 		$query->condition('attendee_id', $row['attendee_id'], '=');		
			$query->condition('event_id', $event_id, '=');				
			$query->condition('change_date', $row['change_date'], '<');
 			_eventbrite_sboc_debug('_eventbrite_sboc_display_sql', array('query' => $query));				
			$query->execute();  
	  }	
	}catch(Exception $e){
		watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
	}	
 }


/**
 * @Database
 * Function _eventbrite_sboc_insert_attendees_to_db
 *   Insert attendee records in a batch to database
 * @param Array rows
 *	 Array of attendee records
 * @return 
 *	 None
 */ 
 function _eventbrite_sboc_insert_attendees_to_db($rows){
 	try{
    	$query = db_insert('eventbrite_sboc_attendees')
	  		->fields(array(
	  			'event_id', 
		  	    'attendee_id', 
		   	    'create_date', 
		   	    'change_date',
	  			'order_id', 
		        'email_address',
	  			'last_name', 
	  			'first_name', 
	  			'category', 
	  			'order_type',
	  			'email_sent', 
	  			'email_send_date',
	  			'reg_type',
	  			'region_name',
	  			'gender',
	 			'school',	 		
	 			'grade',
	 			'year_of_birth',
	 			'month_of_birth',
	 			'day_of_birth',	 		
	 			'home_address_line_1',
	 			'home_address_line_2',
	 			'home_city',
	 			'home_prov_state',
	 			'home_postal_zip',
	 			'home_phone_1',
	 			'home_phone_2',
	 			'additional_info',
	 			'contestant_last_name',
	 			'contestant_first_name',
	 			'email_consent',
	 			'ticket_id',
	 			));
	  
		foreach($rows as $row){
	  		$query->values($row); 
    	}	
	
		$query->execute();  
	}catch(Exception $e){
		watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
	}	
 }
 
 /**
 * @Database 
 * Function _eventbrite_sboc_insert_attendee_to_db
 *   Insert a single attendee record to database
 * @param Array rows
 *	 Array of attendee records
 * @return 
 *	 None
 */
 function _eventbrite_sboc_insert_attendee_to_db($attendee){
 	$eid = 0;
 	try{
   		$eid = db_insert('eventbrite_sboc_attendees')
			     ->fields(array(
				    'event_id' => $attendee->event_id,
     				'attendee_id' => $attendee->id,
// 	 				'create_date' => _eventbrite_sboc_get_timestamp(),
					'create_date' => $attendee->created,
	 				'change_date' => $attendee->changed,
	 				'order_id' => $attendee->order_id,
	 				'email_address' => $attendee->email,
	 				'last_name' => $attendee->last_name,
	 				'first_name' => $attendee->first_name,
	 				'category' => $attendee->category, 
	 				'order_type' => $attendee->order_type, 
	 				'email_sent' => $attendee->email_sent, 
	 				'reg_type' =>  $attendee->registration_type,
	 				'region_name' => $attendee->region,
	 				'gender' => $attendee->gender,	 
	 				'school' => $attendee->school,	 		
	 				'grade' => $attendee->grade,
	 				'year_of_birth' => $attendee->yob,
			 		'month_of_birth' => $attendee->mob,
			 		'day_of_birth' => $attendee->dob,	 		
			 		'home_address_line_1' => $attendee->address_line_1,
			 		'home_address_line_2' => $attendee->address_line_2,
			 		'home_city' => $attendee->address_city,
			 		'home_prov_state' => $attendee->address_prov_state,
			 		'home_postal_zip' => $attendee->address_postal_zip,
			 		'home_phone_1' => $attendee->phone_1,
			 		'home_phone_2' => $attendee->phone_2,
			 		'additional_info' => $attendee->additional_info,
			 		'contestant_last_name' => $attendee->child_last_name,
			 		'contestant_first_name' => $attendee->child_first_name,
			 		'email_consent' => ($a->email_consent == 'Yes' ? 1 : 0),
			 		'ticket_id' => $a->ticket_id,
			  ))
  			  ->execute();
  	}catch(Exception $e){
  	   watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
  	}		  
  	return $eid;
 }
 
 /**
 * @Database
 * Function _eventbrite_sboc_message_body
 *   Retrieves email message from Drupal node tables
 * @param 
 *	 None
 * @return String output
 *	 Node body or error message
 */
 function _eventbrite_sboc_message_body(){
   $output = '<p style="font-weight: bold;">'. t('Message could not be retrieved at this time.'). '</p>';
   $title = variable_get('eventbrite_sboc_content_title', '');
   $query = new EntityFieldQuery();
   $entities = $query->entityCondition('entity_type', 'node')
   ->propertyCondition('title', $title)
   ->range(0,1)
   ->execute();
   if (!empty($entities['node'])){
	 $node = node_load((int)current($entities['node'])->nid);
	 if (is_object($node)){
	   $lang = language_default();
	   $output = $node->body[$lang->language][0]['value'];
	 }
   }
   return $output;
 }
 
 /**
 * @Database
 * Function _eventbrite_sboc_get_change_date_from_db
 *   Retrieves change date for attendee record id from database
 * @param Array rows
 *  Array of attendee rows from database
 * @param Object a
 *	 Attendee record to be cached
 * @return String result
 *	 Date as a string
 */
function _eventbrite_sboc_get_change_date_from_db($rows, $a){
   	$result = '';
   	foreach($rows as $key => $row) {
   		if ($a->id == $key){
   			$result = $row['change_date'];	
   			break;
   		}
   	}					
   	return $result;
 }
 
 /**
 * @Database
 * Function _eventbrite_sboc_get_field_value_from_db
 *   Retrieves change date for attendee record id from database
 * @param Array rows
 *  Array of attendee rows from database
 * @param Object a
 *	 Attendee record to be matched using Attendee Id
 * @param String
 *   Field name to be retrieved
 * @return String result
 *	 Date as a string
 */
function _eventbrite_sboc_get_field_value_from_db($rows, $a, $field_name){
   	$result = '';
   	foreach($rows as $key => $row) {
   		if ($a->id == $key){
   			if (isset($row[$field_name])){
   			   $result = $row[$field_name];	
   			}	
   			break;
   		}
   	}					
   	return $result;
 }