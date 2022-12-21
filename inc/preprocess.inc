<?php

/**
 * @file
 * Theme hook_preprocess functions.
 */

use Drupal\block\Entity\Block;
use Drupal\Core\Datetime\DrupalDateTime;
use Drupal\Core\Template\Attribute;
use Drupal\Core\Render\Markup;
use Drupal\Component\Utility\Html;
use Drupal\media\MediaInterface;
use Drupal\nicsdru_dept_theme\Helpers;
use Drupal\nicsdru_dept_theme\LayoutClasses;
use Drupal\node\Entity\Node;
use Drupal\node\NodeInterface;

/**
 * Implements hook_preprocess_html().
 */
function nicsdru_dept_theme_preprocess_html(array &$variables) {

  // Set body classes for specific departments.
  $dept_manager = \Drupal::service('department.manager');
  switch ($dept_manager->getCurrentDepartment()->id()) {
    case 'group_2':
      $variables['attributes']['class'][] = 'dept-dof';
      break;
    case 'group_3':
      $variables['attributes']['class'][] = 'dept-daera';
      break;
    case 'group_4':
      $variables['attributes']['class'][] = 'dept-dfc';
      break;
    case 'group_5':
      $variables['attributes']['class'][] = 'dept-dfe';
      break;
    case 'group_6':
      $variables['attributes']['class'][] = 'dept-de';
      break;
    case 'group_7':
      $variables['attributes']['class'][] = 'dept-teo';
      break;
    case 'group_8':
      $variables['attributes']['class'][] = 'dept-doh';
      break;
    case 'group_9':
      $variables['attributes']['class'][] = 'dept-dfi';
      break;
    case 'group_10':
      $variables['attributes']['class'][] = 'dept-doj';
      break;
    default:
      $variables['attributes']['class'][] = 'dept-nigov';
      break;
  }

  // Set body classes for specific routes.
  $route = \Drupal::routeMatch()->getRouteName();

  switch ($route) {
    case "dept_gp.gp_search":
      // Set a body class on the GP practice search page.
      $variables['attributes']['class'][] = 'page-search';
      break;
  }
}

/**
 * Implements hook_preprocess_page().
 */
function nicsdru_dept_theme_preprocess_page(array &$variables) {

  // Process node pages.
  if (!empty($variables['node'])) {

    $node = $variables['node'];

    if (method_exists($node, 'bundle')) {
      switch ($node->bundle()) {
        // Specific node types have a main and aside.
        case "article":
        case "application":
        case "health_condition":
        case "news":
        case "page":
          // If page is not in a book, it has a main and aside.
          if (empty($node->book)) {
            $variables['main_content_attr']->addClass(LayoutClasses::MAIN_SIDE);
            $variables['sidebar_second_attr']->addClass('ga-aside');
          }
          else {
            // Pages in a book have a main and the sidebar is removed.
            $variables['main_content_attr']->addClass(LayoutClasses::MAIN_LIMITED);
            unset($variables['page']['sidebar_second']);
          }
          break;

        // Specific node types have a main only.
        case "publication":
        case "embargoed_publication":
        case "landing_page":
        case "consultation":
          $variables['main_content_attr']->addClass(LayoutClasses::MAIN);
          break;

        // All the rest have a main with a limited width (includes
        // contact and gp_practice).
        default:
          $variables['main_content_attr']->addClass(LayoutClasses::MAIN_LIMITED);
          break;
      }
    }
  }

  // Process search view pages.
  $viewpage = \Drupal::routeMatch()->getParameter('view_id');

  // Will need bits and pieces from the request.
  $request = \Drupal::request();

  if (isset($viewpage)) {
    switch ($viewpage) {

      case "publications_search":
        $variables['main_content_attr']->addClass(LayoutClasses::SIDE_MAIN);
        $variables['sidebar_first_attr']
          ->addClass('ga-aside')
          ->setAttribute('role', 'search')
          ->setAttribute('aria-roledescription', t('search publications'));
        break;

      case "consultations_search":
        $variables['main_content_attr']->addClass(LayoutClasses::SIDE_MAIN);
        $variables['sidebar_first_attr']
          ->addClass('ga-aside')
          ->setAttribute('role', 'search')
          ->setAttribute('aria-roledescription', t('search consultations'));
        break;

      case "news_search":
        $variables['main_content_attr']->addClass(LayoutClasses::SIDE_MAIN);
        $variables['sidebar_first_attr']
          ->addClass('ga-aside')
          ->setAttribute('role', 'search')
          ->setAttribute('aria-roledescription', t('search news'));
        break;

      case "press_release_search":
        $variables['main_content_attr']->addClass(LayoutClasses::SIDE_MAIN);
        $variables['sidebar_first_attr']
          ->addClass('ga-aside')
          ->setAttribute('role', 'search')
          ->setAttribute('aria-roledescription', t('search press releases'));
        break;

      case "news_search":
        $variables['main_content_attr']->addClass(LayoutClasses::SIDE_MAIN);
        $variables['sidebar_first_attr']
          ->addClass('ga-aside')
          ->setAttribute('role', 'search')
          ->setAttribute('aria-roledescription', t('search news'));
        break;

      case "search":
        $variables['main_content_attr']->addClass(LayoutClasses::MAIN_LIMITED);
        if ($variables['content_wrapper_attr'] instanceof Attribute) {
          $variables['content_wrapper_attr']
            ->addClass(['ga-main', 'search-content'])
            ->setAttribute('role', 'presentation');
        }
        break;
    }
  }

  // Process pages based on routes.
  $route = \Drupal::routeMatch()->getRouteName();

  switch ($route) {
    // Contacts page uses multiple routes defined in dept_contacts module.
    case 'dept_contacts.default':
    case 'dept_contacts.letter':
      $variables['main_content_attr']->addClass(LayoutClasses::MAIN_LIMITED);
      if ($variables['content_wrapper_attr'] instanceof Attribute) {
        $variables['content_wrapper_attr']
          ->addClass(['ga-main', 'list-content'])
          ->setAttribute('role', 'presentation');
      }
      break;

    // News landing page.
    case 'dept_news.news_listing':
      $variables['main_content_attr']->addClass(LayoutClasses::MAIN);
      if ($variables['content_wrapper_attr'] instanceof Attribute) {
        $variables['content_wrapper_attr']
          ->addClass(['ga-main', 'list-content'])
          ->setAttribute('role', 'presentation');
      }
      break;

    // Site theme term pages.
    case 'entity.taxonomy_term.canonical':
      // Set the section css classes for theme taxonomy terms.
      if ($variables['term']->bundle() === 'site_themes') {
        $variables['main_content_attr']->addClass([
          'grid',
          'layout--theme-term',
        ]);
        if ($variables['content_wrapper_attr'] instanceof Attribute) {
          $variables['content_wrapper_attr']
            ->addClass(['ga-main', 'list-content'])
            ->setAttribute('role', 'presentation');
        }
        $variables['sidebar_second_attr']->addClass('section--popular');
      }
      break;

    // Webforms.
    case 'entity.webform.canonical':
      $variables['main_content_attr']->addClass(LayoutClasses::MAIN_LIMITED);
      if ($variables['content_wrapper_attr'] instanceof Attribute) {
        $variables['content_wrapper_attr']
          ->addClass(['ga-main', 'form-content'])
          ->setAttribute('role', 'presentation');
      }
      break;

    // Node preview pages.
    // Ensure the side-bar in the non node canonical views are visible to
    // accurately reflect how the published node will look.
    case 'entity.node.preview':
    case 'entity.node.revision':
    case 'entity.node.latest':
      $node_id = \Drupal::routeMatch()->getRawParameter('node');
      $node = Node::load($node_id);

      if ($node === NULL) {
        $node = \Drupal::routeMatch()->getParameter('node_preview');
      }

      $node_type = $node->getType();

      switch ($node_type) {
        case 'news':
          // Render Featured and Latest news block.
          $variables['page']['sidebar_second'][] = Helpers::blockContent('featurednews');
          $variables['page']['sidebar_second'][] = Helpers::blockContent('views_block__news_latest_news_block');

        case 'article':
        case 'news':
        case 'application':
        case 'landing_page':
        case 'page':

          // Render the Related Content block.
          // We can't use Helpers::blockContent() here as we are loading a
          // plugin block instead of a content block.
          $block_manager = \Drupal::service('plugin.manager.block');
          $plugin_block = $block_manager->createInstance('dept_related_content_block', []);
          $block_label = 'Related content';

          // Applying title as hook_block_view_alter() which applies the title
          // for canonical routes doesn't get called here.
          if ($node->hasField('field_subtheme') && !$node->get('field_subtheme')->isEmpty()) {
            $block_label = $node->get('field_subtheme')->entity->getName();
          }

          // We have to create a block render array as $plugin_block->build()
          // will only create the inner block content.
          $variables['page']['sidebar_second'][] = [
            '#theme' => 'block',
            '#weight' => 0,
            '#configuration' => [
              'label' => $block_label,
              'label_display' => '1',
            ],
            '#plugin_id' => $plugin_block->getPluginId(),
            '#derivative_plugin_id' => NULL,
            'content' => $plugin_block->build(),
          ];
          break;

        case 'health_condition':
          // Render Health Conditions search block.
          $variables['page']['sidebar_second'][] = Helpers::blockContent('exposedformhealth_conditionssearch_sidebar_2');

          // Render Health Conditions AtoZ block.
          $variables['page']['sidebar_second'][] = Helpers::blockContent('healthconditionsatoz_sidebar_2');

          // Render Health Conditions Related block.
          $variables['page']['sidebar_second'][] = Helpers::blockContent('healthconditionsrelatedconditions');
          break;

        default:
          break;
      }

      // Add classes to main and sidebar regions to listed node types.
      switch ($node_type) {
        case "article":
        case "application":
        case "health_condition":
        case "news":
        case "page":
          $variables['main_content_attr']->addClass(LayoutClasses::MAIN_SIDE);
          $variables['sidebar_second_attr']->addClass('ga-aside');
          break;

        // Specific node types have a main only.
        case "publication":
        case "embargoed_publication":
        case "landing_page":
        case "consultation":
          $variables['main_content_attr']->addClass(LayoutClasses::MAIN);
          break;

        // All the rest have a main with a limited width (includes
        // contact and gp_practice).
        default:
          $variables['main_content_attr']->addClass(LayoutClasses::MAIN_LIMITED);
          break;
      }

      break;

    // Health conditions A to Z letter page
    // (see dept_health_conditions module).
    case 'dept_health_conditions.letter':
      $variables['main_content_attr']->addClass(LayoutClasses::SIDE_MAIN);
      if ($variables['content_wrapper_attr'] instanceof Attribute) {
        $variables['content_wrapper_attr']
          ->addClass(['ga-main', 'search-content'])
          ->setAttribute('role', 'presentation');
      }
      $variables['sidebar_first_attr']
        ->addClass('ga-aside')
        ->setAttribute('role', 'search')
        ->setAttribute('aria-roledescription', t('search health conditions or choose a letter from the A to Z'));
      break;

    default:
      break;
  }

  if (isset($variables['is_front']) && $variables['is_front'] == TRUE) {
    $front_classes = ['grid', 'layout--front'];
    $variables['main_content_attr']->addClass($front_classes);
    $variables['sidebar_second_attr']->addClass('section--popular');
  }
}

/**
 * Implements hook_preprocess_node().
 */
function nicsdru_dept_theme_preprocess_node(array &$variables) {

  // Array of bundles for which the social sharing block should be displayed.
  $social_bundles = [
    'application',
    'article',
    'contact',
    'consultation',
    'embargoed_publication',
    'health_condition',
    'news',
    'application',
    'landing_page',
    'page',
    'publication',
    'gp_practice',
  ];

  $variables['display_social_links'] = in_array($variables['node']->bundle(), $social_bundles);

  $variables['show_title'] = TRUE;
  $variables['show_topics'] = TRUE;

  switch ($variables['node']->getType()) {
    case "article":
    case "application":
    case "contact":
    case "gp_practice":
    case "health_condition":
    case "news":
    case "publication":
    case "embargoed_publication":
    case "subtopic":
      $variables['attributes']['class'][] = Html::cleanCssIdentifier('ga-main');
      $variables['attributes']['class'][] = Html::cleanCssIdentifier('article-content');
      break;

    case "webform":
      $variables['attributes']['class'][] = Html::cleanCssIdentifier('ga-main');
      break;

    case "feature":
      // Unset any fields which have a specific preprocess variable.
      // See nicsdru_dept_theme_preprocess_field() for an example.
      if (!empty($variables['elements']['#exclude_fields'])) {
        foreach ($variables['elements']['#exclude_fields'] as $field_id) {
          unset($variables['content'][$field_id]);
        }
      }

      if (isset($variables['elements']['#photo_image_style'])) {
        // Some features use a different image style, eg: top set
        // of featured content on the homepage.
        $variables['content']['field_photo'][0]['#view_mode'] = $variables['elements']['#photo_image_style'];
      }
      break;

    case "featured_content_list":
      // Only show the title on the canonical node route.
      if (\Drupal::routeMatch()->getRouteName() !== 'entity.node.canonical') {
        $variables['show_title'] = FALSE;
      }

      break;

    case "publication":
      // Change field label to 'Documents'.
      $variables['content']['field_attachment']['#title'] = t('Documents');
      break;

    case "landing_page":
    case "page":
      // Omit the title on these content types; causes trouble with search
      // indexing. Resolvable with either suitable block config or specific
      // node templates for these types.
      $variables['show_title'] = FALSE;
      break;
  }

  // Gather a list of topics for the node, if needed, which can be
  // used in the node template to give a controllable list of topics
  // and optional subtopics ordered by title.
  if ($variables['show_topics'] && $variables['node'] instanceof NodeInterface) {
    $topics = \Drupal::service('dept_node.topics')
      ->getTopics($variables['node'], TRUE, TRUE);

    if (!empty($topics)) {
      $variables['topics_subtopics_list'] = [
        '#theme' => 'topics_subtopics_list',
        '#title' => t('Topics'),
        '#items' => $topics,
        '#render_links' => TRUE,
        '#include_subtopics' => TRUE,
      ];
    }
  }
  // Only use this code for consultations content type.
  if ($variables['node']->getType() === 'consultation') {

    // Get the current time to gauge consultation start and end dates off.
    $current_time = \Drupal::time()->getCurrentTime();

    // Convert consultation start date to a timestamp.
    $consultation_start_date = $variables['node']->get('field_consultation_dates')->value;
    $consultation_start_date_date_time = new DrupalDateTime($consultation_start_date, 'UTC');
    $consultation_start_date_timestamp = $consultation_start_date_date_time->getTimestamp();

    // Convert consultation end date to a timestamp.
    $consultation_end_date = $variables['node']->get('field_consultation_dates')->end_value;
    $consultation_end_date_date_time = new DrupalDateTime($consultation_end_date, 'UTC');
    $consultation_end_date_timestamp = $consultation_end_date_date_time->getTimestamp();

    // Set a variable to be used in the twig template for the consultation end date time.
    $variables['consultation_end_time'] = date('H:i', $consultation_end_date_timestamp);

    // Set a variable to be used in the twig template for the ways to respond fieldgroup, initially set to 0.
    $variables['consultation_in_progress'] = FALSE;

    // If the consultation started before and is ending after today's date then show the contact information.
    if ($current_time > $consultation_start_date_timestamp && $current_time <= $consultation_end_date_timestamp) {
      $variables['consultation_in_progress'] = TRUE;
      // Text to display if the consultation is open or closed. Used in the search result view mode only.
      $output = t('Open');
    }
    else {
      $output = t('Closed');
    }
  }
}

/**
 * Implements hook_preprocess_block().
 */
function nicsdru_dept_theme_preprocess_block(array &$variables) {

  // Set department logo for system branding block.
  if ($variables['plugin_id'] === 'system_branding_block') {
    $dept_manager = \Drupal::service('department.manager');
    switch ($dept_manager->getCurrentDepartment()->id()) {
      case 'group_2':
        $variables["department_logo"] = 'dof.svg';
        break;
      case 'group_3':
        $variables["department_logo"] = 'daera.svg';
        break;
      case 'group_4':
        $variables["department_logo"] = 'dfc.svg';
        break;
      case 'group_5':
        $variables["department_logo"] = 'dfe.svg';
        break;
      case 'group_6':
        $variables["department_logo"] = 'de.svg';
        break;
      case 'group_7':
        $variables["department_logo"] = 'teo.svg';
        break;
      case 'group_8':
        $variables["department_logo"] = 'doh.svg';
        break;
      case 'group_9':
        $variables["department_logo"] = 'dfi.svg';
        break;
      case 'group_10':
        $variables["department_logo"] = 'doj.svg';
        break;
      default:
        $variables["department_logo"] = 'niexec.svg';
        break;
    }
  }

  // Set the title for 'Most popular by term' block.
  $route = \Drupal::routeMatch();
  if ($variables['plugin_id'] === 'views_block:popular_content-pop_by_term' && $route->getRouteName() === 'entity.taxonomy_term.canonical') {
    $variables['label'] = t('Popular in @term', ['@term' => strtolower($route->getParameter('taxonomy_term')->label())]);
  }

  // Add nav-menu class Related Content and Conditions blocks.
  if ($variables['plugin_id'] === 'dept_related_content_block') {
    $variables['content']['related_content']['#attributes']['class'] = ['nav-menu'];
  }
  elseif ($variables['plugin_id'] === 'healthconditions_related_conditions') {
    $variables['content']['related_conditions']['#attributes']['class'] = ['nav-menu'];
  }

}

/**
 * Implements hook_preprocess_page_title().
 */
function nicsdru_dept_theme_preprocess_page_title(&$variables) {

  // By default page titles are visible.
  $variables['title_visible'] = TRUE;

  // Set the home page title and visually hide it.
  if (\Drupal::service('path.matcher')->isFrontPage()) {
    $variables['title'] = t('Welcome to N I Direct');
    $variables['title_visible'] = FALSE;
  }

  // Preprocess titles on certain routes.
  $route = \Drupal::routeMatch();

  // Will need bits and pieces from the request for titles.
  $request = \Drupal::request();

  switch ($route->getRouteName()) {

    // Preprocess search page title.
    case "view.search.search_page":
      // Modify title if a search query is present.
      $query = $request->get('query');
      if (!empty($query)) {
        $variables['title'] = t('Search results');
      }
      // Add a subtitle to indicate the current page selected from the pager.
      $page = $request->get('page');
      if (!empty($page) && $page > 0) {
        $page = ++$page;
        $variables['title_subtitle'] = t('page :page', [':page' => $page]);
      }
      break;

    // Preprocess news listing page title.
    case "dept_news.news_listing":
      // Add a subtitle to indicate the current page selected from the pager.
      $page = $request->get('page');
      if (!empty($page) && $page > 0) {
        $page = ++$page;
        $variables['title_subtitle'] = t('page :page', [':page' => $page]);
      }
      break;

    // Preprocess contacts search page title.
    case "dept_contacts.default":
      // Add a subtitle to indicate the current page selected from the pager.
      $page = $request->get('page');
      if (!empty($page)) {
        $page = ++$page;
        $variables['title_subtitle'] = t('page :page', [':page' => $page]);
      }
      break;

    // Preprocess contacts A to Z page title.
    case "dept_contacts.letter":
      // Add a subtitle to indicate letter chosen from the A to Z.
      $letter = $route->getParameter('letter');
      if (!empty($letter)) {
        $variables['title'] = 'Contacts';
        $variables['title_subtitle'] = t('under :letter', [':letter' => strtoupper($letter)]);
      }
      break;

    // Preprocess health conditions search page title.
    case "view.health_conditions.search_page":
      // Add a subtitle to indicate showing results and the current page
      // selected from the pager.
      $page = $request->get('page');
      $search_query = $request->get('query_health_az');
      if (!empty($search_query) && empty($page)) {
        $variables['title_subtitle'] = t('search results');
      }
      if (!empty($search_query) && !empty($page)) {
        $page = ++$page;
        $variables['title_subtitle'] = t('search results page :page', [':page' => $page]);
      }
      break;

    // Preprocess health conditions A to Z listing title.
    case "dept_health_conditions.letter":
      // Add a subtitle to indicate letter chosen from the A to Z.
      $letter = $route->getParameter('letter');
      if (!empty($letter)) {
        $variables['title_subtitle'] = t('conditions under :letter', [':letter' => strtoupper($letter)]);
      }
      break;

    // Preprocess GP practice search page title.
    case "dept_gp.gp_search":
      // Add a subtitle to indicate the current page selected from the pager.
      $page = $request->get('page');
      if (!empty($page)) {
        $page = ++$page;
        $variables['title_subtitle'] = t('page :page', [':page' => $page]);
      }
      break;

    // Preprocess driving instructor search page title.
    case "view.driving_instructors.driving_instructor_results":
      // Add a subtitle to indicate showing results and the current page
      // selected from the pager.
      $page = $request->get('page');
      $search_query = $request->get('search_api_fulltext');
      if (!empty($search_query) && empty($page)) {
        $variables['title'] = t('Approved driving or motorcycle instructors');
        $variables['title_subtitle'] = t('search results');
      }
      elseif (!empty($search_query) && !empty($page)) {
        $page = ++$page;
        $variables['title'] = t('Approved driving or motorcycle instructors');
        $variables['title_subtitle'] = t('search results page :page', [':page' => $page]);
      }
      break;
  }
}

/**
 * Implements hook_preprocess_field().
 */
function nicsdru_dept_theme_preprocess_field(&$variables) {
  if ($variables['element']['#field_name'] === 'field_featured_content' && \Drupal::service('path.matcher')->isFrontPage()) {
    // Only show 3 items on the homepage.
    if (count($variables['items']) > 3) {
      $variables['items'] = array_slice($variables['items'], 0, 3);
    }

    // If we're rendering this field on the front page, see if the parent object
    // id matches the node id defined for the feature COVID-19 FCL node. If it
    // does then we want to add a preprocess variable to let us hide the teaser
    // field which saves having to define a whole new set of view modes + config
    // for a really small trivial change to the way the feature nodes render.
    $block = Block::load('featuredcontent_covid19');
    $covid_fcl_nid = $block->get('settings')['featured_items'];
    $parent = &$variables['element']['#object'];

    if ($parent instanceof NodeInterface && $parent->id() == $covid_fcl_nid) {
      foreach ($variables['items'] as $key => &$feature_node) {
        $feature_node['content']['#exclude_fields'] = ['field_teaser'];
        // Set custom field_photo image style.
        $feature_node['content']['#photo_image_style'] = 'article_full';
      }
    }
  }

  // Add a class to the anchor tag on application link fields & append the icon.
  if (isset($variables['element'])
    && $variables['element']['#field_name'] === 'field_link'
    && $variables['element']['#bundle'] === 'application'
  ) {
    foreach ($variables['items'] as $key => $item) {
      $icon = '<svg aria-hidden="true" class="ico ico-arrow-right"><use xlink:href="#arrow"></use></svg>';
      $title = $item['content']['#title'];
      $classes = ['call-to-action'];

      $formattedTitle = Markup::create($title . $icon);

      $variables['items'][0]['content']['#title'] = $formattedTitle;
      $variables['items'][0]['content']['#url']->setOption('attributes', ['class' => $classes]);
    }
  }
  // Amend classes on the standard address field output.
  if (isset($variables['element'])
    && $variables['element']['#field_type'] == 'address'
  ) {
    $item_count = count($variables['items']);
    for ($i = 0; $i < $item_count; $i++) {
      $variables['items'][$i]["content"]["#prefix"] = '<p class="adr" translate="no">';
      if (($key = array_search('address-line1', $variables['items'][$i]['content']['address_line1']['#attributes']['class'])) !== FALSE) {
        unset($variables['items'][$i]['content']['address_line1']['#attributes']['class'][$key]);
      }
      $variables['items'][$i]['content']['address_line1']['#attributes']['class'][] = 'street-address';
      if (($key = array_search('address-line2', $variables['items'][$i]['content']['address_line2']['#attributes']['class'])) !== FALSE) {
        unset($variables['items'][$i]['content']['address_line2']['#attributes']['class'][$key]);
      }
      $variables['items'][$i]['content']['address_line2']['#attributes']['class'][] = 'street-address';
      $i++;
    }
  }

  if (isset($variables['element'])
    && $variables['element']['#field_name'] === 'body'
    && $variables['element']['#bundle'] === 'webform'
  ) {
    $variables['body_summary'] = $variables['element']['#object']->body->summary;
  }
}

/**
 * Implements hook_preprocess_views_view().
 */
function nicsdru_dept_theme_preprocess_views_view(&$variables) {
  // Amend the role / labelling for search results pages.
  if ($variables['id'] === 'publications' && $variables['display_id'] === 'search_page') {
    $search_classes = [
      'card-deck',
      'border-bottom',
      'search-results',
    ];
    if (!isset($variables['attributes']['class'])) {
      $variables['attributes']['class'] = [];
    }
    $variables['attributes']['class'] = array_merge($variables['attributes']['class'], $search_classes);
    $variables['attributes']['role'] = 'region';
    $variables['attributes']['aria-label'] = t('search results');
  }
  if ($variables['view']->current_display === 'contacts_by_letter') {
    // Do not show search box if we are in a letter search.
    $route = \Drupal::routeMatch()->getRouteName();
    if ($route === 'dept_contacts.letter') {
      $variables['exposed'] = NULL;
    }
  }
  if ($variables['id'] === 'popular_content' && $variables['display_id'] === 'pop_by_term') {
    $variables['attributes']['class'][] = 'sub-menu';
    $variables['attributes']['class'][] = 'sub-menu--popular';
  }
}

/**
 * Implements hook_preprocess_form().
 */
function nicsdru_dept_theme_preprocess_form(array &$variables) {
  $view = $variables['element']['#id'];
  switch ($view) {
    case "views-exposed-form-contacts-a-z-contacts-by-letter":
    case "views-exposed-form-contacts-contact-search":
    case "views-exposed-form-publications-search-publications-search":
    case "views-exposed-form-consultations-search-consultations-search":
    case "views-exposed-form-news-search-news-search":
    case "views-exposed-form-press-release-search-pr-search-page":
      $variables['attributes']['class'][] = Html::cleanCssIdentifier('search-form');
      $variables['attributes']['class'][] = Html::cleanCssIdentifier('search-form--page');
      break;
  }
}

/**
 * Implements hook_preprocess_responsive_image_formatter().
 */
function nicsdru_dept_theme_preprocess_responsive_image_formatter(array &$variables) {
  // Load the parent media entity that the responsive image is
  // or might be associated to.
  $media_image = $variables['item']->getEntity();

  if ($media_image instanceof MediaInterface === FALSE) {
    return;
  }

  // If we've got a parent media entity and caption field value
  // then inject the value into the responsive image attributes collection.
  // We're relying on Drupal input sanitising from the media form as
  // well as safe Twig rendering in responsive-image.html.twig to cover
  // any content that is migrated in from the legacy db and could
  // potentially contain dodgy html.
  $caption = $media_image->field_caption->value;

  if (!empty($caption)) {
    $variables['responsive_image']['#attributes']['caption'] = $caption;
  }
}

/**
 * Implements template_preprocess_container().
 */
function nicsdru_dept_theme_preprocess_container(&$variables) {
  // Fix for HTML validation error caused by duplicate "edit-actions" ID present
  // in header search form and in the page feedback form.
  // @see https://www.drupal.org/project/drupal/issues/1852090.
  if (isset($variables['element']['#id'])) {
    $id = $variables['element']['#id'];
    if (in_array($id, ['edit-actions'])) {
      $id = Html::getUniqueId($id);
    }
    $variables['element']['#id'] = $id;
    $variables['attributes']['id'] = $id;
  }
}

/**
 * Implements hook_preprocess_HOOK().
 */
function nicsdru_dept_theme_preprocess_views_view_rss(&$variables) {
  // Based on https://jungleran.com/node/44 for preprocess
  // code to refine the output of the news RSS feed.
  $view = $variables['view'];
  if ($view->current_display === 'news_feed' && $view->id() === 'news') {
    // The 'link' in the preprocess variables doesn't give us the full path so
    // we can create a new one here to give us what we need.
    $display = $view->getDisplay();
    $variables['feed_url'] = $display->getUrl()->setAbsolute()->toString();
    // Alter namespaces to include the atom namespace as recommended
    // by the W3C validator service.
    $style = $view->getStyle();
    $style->namespaces = array_filter($style->namespaces, function ($key) {
      return $key === 'xmlns:dc';
    }, ARRAY_FILTER_USE_KEY);

    $style->namespaces['xmlns:atom'] = 'http://www.w3.org/2005/Atom';
    $variables['namespaces'] = new Attribute($style->namespaces);
  }
}

/**
 * Implements hook_preprocess_region().
 */
function nicsdru_dept_theme_preprocess_region(&$variables) {
  // Make the banner image and banner link fields in subtopic available in the top_banner region.
  if ($variables['region'] == 'top_banner') {
    if ($node = \Drupal::routeMatch()->getParameter('node')) {
      if ($node->getType() == 'subtopic') {
        $variables['is_subtopic'] = TRUE;

        if ($node->hasField('field_banner_image') && !$node->get('field_banner_image')->isEmpty()) {
          $variables['field_banner_image'] = $node->get('field_banner_image')->view('default');
        }

        if ($node->hasField('field_banner_link') && !$node->get('field_banner_link')->isEmpty()) {
          $variables['field_banner_link'] = $node->get('field_banner_link')->view('default');
        }
      }
    }
  }

  // Make the title field text available in the related region.
  if ($variables['region'] == 'related') {
    if ($node = \Drupal::routeMatch()->getParameter('node')) {
      if ($node->getType() == 'subtopic') {
        $variables['subtopic_field_title'] = $node->get('title')->view();
      }
    }
  }
}
