
(function($) {

  Drupal.ajax.prototype.triggeredResponse = function() {
    if (this.ajaxing) {
      return false;
    }
    try {
      $.ajax(this.options);

    } catch (err) {
      return false;
    }
    return false;
  };

  function twoway_status_update_next() {
    if ($(".twoway_status_request").not(".ajax-changed").size() > 0) {
      $(".twoway_status_request")
        .not(".ajax-changed")
        .filter(":first")
        .each(function() {
          $(this).html('<span class="twoway_connecting">Connecting...</span>');
          $.ajax({
            url: '?q=twoway/get_status/' + $(this).attr('id'),
            cache: false,
            dataType: 'json',
            success: function(data) {
              $("#" + data.id).html(data.replace);
              $("#" + data.id).addClass("ajax-changed");
              $("#services_" + data.id).text(data.total);
              twoway_status_update_next()
            }
          });
      });
    }
  }

  $(document).ready(function() {
    twoway_status_update_next();
  });

}) (jQuery);