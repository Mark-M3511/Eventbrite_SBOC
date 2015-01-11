<?php

 /**
 * Implementation of hook_theme
 * @param array form
 *	 Form values as a drupal style array
 * @param array form_state
 *	 Form values in changed state as a drupal style array
 * @param string form_state
 *	 Unique form identifier
 */
 function eventbrite_sboc_theme($existing, $type, $theme, $path){
    return array(
	   'login_widget' => array (		
		  'template' => 'eventbrite_sboc_login',
		  'path' => drupal_get_path('module', 'eventbrite_sboc') .'/templates',
	   ),
	   'attendee_list' => array(
	      'variables' => array('attendee_list' => NULL, 'attendee_list_count' => NULL),
	   ),
	   'attendee_list_from_db' => array(
	      'variables' => array('attendee_list' => NULL, 'attendee_list_count' => NULL, 'start_date' => NULL,'end_date' => NULL),
	   ),
	   'process_complete' => array(
	   	   'variables' => array('msg' => NULL, 'count_new' => 0, 'count_changed' => 0,'email_count' => 0,),
	   ),
	   'process_complete_goto_log' => array(
	   	   'variables' => array('log_path' => NULL,),
	   ),
	);
 }
 
 
 /**
 * Implementation of theme_hook
 * @param array form
 *	 Form values as a drupal style array
 * @param array form_state
 *	 Form values in changed state as a drupal style array
 * @param string form_state
 *	 Unique form identifier
 */
 function theme_attendee_list($variables){
   $attendee_list_count = $variables['attendee_list_count'];
   $attendee_list = $variables['attendee_list'];
   $ra_attendee_list =  array(
	  'attendee_list_count_line' => array(
		 '#type' => 'markup',
		 '#prefix' => '<p><strong>',
		 '#markup' => format_string('Total registered attendees for this event on Eventbrite: !d', array('!d' => $attendee_list_count)),
		 '#suffix' => '</strong></p>',
       ),
	   'attendee_list_table' => array(
	      '#type' => 'markup', 
		  '#markup' => $attendee_list,
	   ),
   );
   $output = drupal_render($ra_attendee_list);
   return $output;
 }
 
 /**
 * Implementation of theme_hook
 * @param array variables
 *	 An array variable names and values, keyed by strings
 * @return string output
 *	 Returns themed output of drupal_render function
 */
 function theme_attendee_list_from_db($variables){
   $attendee_list_count = $variables['attendee_list_count'];
   $start_date = $variables['start_date'];
   $end_date = $variables['end_date'];
   $attendee_list = $variables['attendee_list'];
   $ra_attendee_list =  array(
	  'attendee_list_count_line' => array(
		 '#type' => 'markup',
		 '#prefix' => '<p><strong>',
		 '#markup' => format_string('Date Range: @d1 to @d2', array('@d1' => $start_date, '@d2' => $end_date,)),
		 '#suffix' => '</strong></p>',
       ),
	   'attendee_list_table' => array(
	      '#type' => 'markup', 
		  '#markup' => $attendee_list,
	   ),
   );
   $output = drupal_render($ra_attendee_list);
   return $output;
 }
 
 /**
 * Function theme_process_complete
 *	See hook_theme implementation for more information
 * @param Array variables
 *	 Associative array of values passed in
 * @return String output
 *	 Rendered output as string
 */
 function theme_process_complete($variables){
  
   $msg = $variables['msg'];
   $count_new = (int)$variables['count_new'];
   $count_changed = (int)$variables['count_changed'];
   $email_count = (int)$variables['email_count'];
   
   $form = drupal_get_form('_eventbrite_sboc_process_complete', $msg, 
   			$count_new, $count_changed, $email_count);
   $output = drupal_render($form);	
   
   return $output;
 }
 
 
 /**
 * Function _eventbrite_sboc_summary_table_array
 *   Create table structure for display
 * @param Array rows
 *	 Array of attendee records
 * @return Array result
 *	 Array of values for table rendering
 */
 function _eventbrite_sboc_summary_table_array($attendees){
   $result = array(); 
   $data = array();
   $header = array(t('Order Id'), t('Create Date'), t('Email Address'), t('Last Name'),   	
       t('First Name'), t('Category'), t('Region'), t('E-Manual Sent'), t('Send Date'),);
   
   foreach($attendees as $a){
     $data[] = array($a->order_id, $a->created, $a->email, $a->last_name, $a->first_name, 
	   $a->category, $a->region,(!empty($a->email_sent) ? 'Yes': 'No'), $a->email_send_date);
   }  
   $result = array('header'=>$header, 'rows'=>$data, 'attributes'=>array(), 'caption'=>t('E-Manual Deliver Summary'), 'colgroups'=>array(), 'empty'=>array(), 'sticky'=>array(),);   
   return $result;
 }
 
 /**
 * Function _eventbrite_sboc_sort_pending
 *  Sorts attendee list by attendee id
 * @param Object a
 *	First attendee object for A/B comparison
 * @param Object b
 *	Second attendee object for A/B comparion
 * @@return Integer result
 *	 0 if values are equal; -1 or 1 otherwise
 */
 function _eventbrite_sboc_sort_pending($a, $b){
    $result = -1;
    $sort = strtoupper(check_plain($_GET['sort']));
    $order = t(check_plain($_GET['order']));
	$header = array('#', t('Order Id'), t('Created'), t('Email Address'), t('Last Name'), t('First Name'),
		 t('Category'), t('Region'), t('Changed'), t('Order Status'),);
    $key =  array_search($order, $header);	
	$a_el = strtoupper($a[$key]);
	$b_el = strtoupper($b[$key]);
    if ($key !== FALSE)	{
	   try {
	     if ($a_el == $b_el){
           $result = 0;
         }else{
           if ($sort == 'ASC'){
             $result = ($a_el > $b_el) ? 1 : -1;
	       }else{
	         $result = ($a_el < $b_el) ? 1 : -1;
	       }
         }
       }catch(Exception $e){
          watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
       }
  }
    
    return $result;
 }
 
 
 /**
 * Function _eventbrite_sboc_display_pending_list
 *   Create table structure for display
 * @param Array attendees
 *	 Array of attendee record objects
 * @return String result
 *	Themed table output
 */
 function _eventbrite_sboc_display_pending_list($attendees){
   $result = array(); 
   $data = array();
   $header = array(
       array('data' => '#', 'field' => 'ctr'),
       array('data' => t('Order Id'), 'field' => 'order_id'),
	   array('data' => t('Created'), 'field' => 'create_date'),
       array('data' => t('Email Address'), 'field' => 'email_address'),
       array('data' => t('Last Name'), 'field' => 'last_name'),   	
       array('data' => t('First Name'), 'field' => 'first_name'), 	
       array('data' => t('Category'), 'field' => 'category_name'),	
	   array('data' => t('Region'), 'field' => 'region'),	
	   /* array('data' => t('Amount Paid'), 'field' => 'amount_paid'),	 */
	   array('data' => t('Changed'), 'field' => 'change_date'),			   
	   array('data' => t('Order Status'), 'field' => 'order_type'),			   
    );	
   
   $ctr = 0;	
   $current_page = pager_default_initialize(count($attendees), EVENTBRITE_SBOC_PAGE_LIMIT); 
   
   foreach($attendees as $a){
     $data[] = array(++$ctr, $a->order_id, $a->created, $a->email, strtoupper($a->last_name), strtoupper($a->first_name), $a->category_name, $a->region, /*$a->amount_paid,*/ $a->changed, $a->order_type,);
   }  
   
   $chunks = array_chunk($data, EVENTBRITE_SBOC_PAGE_LIMIT, TRUE); 

   if (isset($_GET['sort']) && isset($_GET['order']) && (count($chunks) > 0)){
	  usort($chunks[$current_page], "_eventbrite_sboc_sort_pending");
   }   
	  
   $link = url('admin/config/system/eventbrite_sboc/api_pending_list');	  
   $caption = '<strong>'. t('Attendees not yet emailed - (!count)  - <a href="!link">New List >>', array('!count' => count($attendees), '!link' => $link)).'</strong>';
  
   $rows = array();
   if (count($chunks) > 0){  
     $rows = $chunks[$current_page];
   }
   
   $result = array('header'=>$header, 'rows'=>$rows, 'attributes'=>array(), 'caption'=> $caption, 'colgroups'=>array(),);       
   $output = theme('table', $result);   
   $output .= theme('pager', array('quantity', count($attendees)));
   
   return $output;
 }
 
 /**
 * Function _eventbrite_sboc_display_attendee_list_from_db
 *   Create table structure for display
 * @param String start_date
 *	 Minimum date value as string
 * @param String end_date
 *	 Maximum date value as string
 * @return String result
 *	 Themed table output
 */
 function _eventbrite_sboc_display_attendee_list_from_db($start_date, $end_date){
   $result = array(); 
   $data = array();
   $header = array( 
        /*  array('data' => t('#'), 'field' => 'eid', 'sort' => 'desc'), */
		 array('data' => t('#'),),
	     array('data' => t('Order Id'), 'field' => 'order_id',),
		 array('data' => t('Order Date'), 'field' => 'create_date',),		 
		 array('data' => t('Email Address'), 'field' => 'last_name',),
		 array('data' => t('Last Name'), 'field' => 'first_name', ),
		 array('data' => t('First Name'), 'field' => 'email_address',), 
		 array('data' => t('Category'), 'field' => 'category',),
         /* array('data' => t('Email Sent'), 'field' => 'email_sent',), */
         array('data' => t('Changed Date'), 'field' => 'change_date',),
		 array('data' => t('Email Sent On'), 'field' => 'email_send_date',),
	);
	
   $attendees = _eventbrite_sboc_get_attendees_from_db_paged($start_date, $end_date, $header);
	
   $ctr = 0;	
   $page_num = _eventbrite_sboc_get_page_num();
   foreach($attendees as $a){
     $data[] = array(++$ctr + (EVENTBRITE_SBOC_PAGE_LIMIT * $page_num), $a->order_id, date('Y-m-d H:i:s', strtotime($a->create_date)), $a->email_address, 
	 strtoupper($a->last_name), strtoupper($a->first_name), $a->category, /*(!empty($a->email_sent) ? 'Yes': 'No'),*/ $a->change_date, $a->email_send_date,);
   } 
   
   $link = url('admin/config/system/eventbrite_sboc/attendee_report');
   $count = _eventbrite_sboc_get_attendees_from_db_paged_count($start_date, $end_date);
   $caption = '<strong>'. t('Attendee count - (!count) - <a href="!link">Run a new report >></a>', array('!count' => $count,
   '!link' => $link)).'</strong>';
  
   $table = array('header'=>$header, 'rows'=>$data, 'attributes'=>array(), 'caption'=> $caption, 'colgroups'=>array(),);    
   
   $attendee_list = theme('table', $table);
   $attendee_list .= theme('pager', array('tags' => array()));
   
   $params = array(
      'attendee_list' => $attendee_list, 
	  'attendee_list_count' => count($attendees),
	  'start_date' => date('Y-M-d',strtotime($start_date)),
	  'end_date' => date('Y-M-d', strtotime($end_date)),
   );	
   return theme('attendee_list_from_db', $params);  
 }
