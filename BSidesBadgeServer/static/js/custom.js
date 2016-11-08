  
  function validateForm()
  {
   

    var now = $('#searchQuery').val();
    var splitTerms = now.split("|");
  
    var lastTerm = splitTerms[splitTerms.length-1].trim().split(" ")[0];
  
    if (lastTerm.toLowerCase().indexOf('table') == -1) {
        alert("All queries must end with '| table <fields>' so that we can perform mapping. Please fix!");
        return false;
    }
    
   var isValid = true;
   
    $('#quickAdd :input').each(function() {
        
      if ( $(this).val() === '' )
      {
          isValid = false;
          console.log($(this))
      }
    });
    
    if (isValid == false) {
        alert("Please fill in all the form details");
        return false;
    }
    
    return true;
  }
  
  function populateTransform(transformid) {
    var result = $.grep(saveSearchAr, function(e){ return e.ID == transformid; });
    if (result.length == 1) {
        thisEntry = result[0];
        $('#searchQuery').val(thisEntry["Query"]);
    }
    
    //console.log(thisEntry["Query"]);
    
    //code
}

function testQuery() {
    
     var queryText = $('#searchQuery').val();
     if (queryText.toLowerCase().indexOf("$$e.value$$") > -1)
     {
        var valuePrompt = prompt("Please enter value for $$E.value$$ to be replaced with!");
        //queryText.replace(new RegExp("$$E.value", "ig"),valuePrompt);
        
        var form = $('<form action="search.php" method="post" target="_blank">' +
  '<input type="hidden" name="searchQuery" value="' + escape(queryText) + '" />' +
  '<input type="hidden" name="earliest_time" value="-1h" />' +
  '<input type="hidden" name="latest_time" value="now" />' +
  '<input type="hidden" name="replaceVal" value="' + escape(valuePrompt) + '" />' +
  '</form>');
  $('body').append(form);
  form.submit();
     }
     else
     {
      var form = $('<form action="search.php" method="post" target="_blank">' +
  '<input type="hidden" name="searchQuery" value="' + escape(queryText) + '" />' +
  '<input type="hidden" name="earliest_time" value="-1h" />' +
  '<input type="hidden" name="latest_time" value="now" />' +
  '</form>');
  $('body').append(form);
  form.submit();
      
     }
     return false;
     
}

function deleteTransform(ID) {
    if(confirm('Are you sure you want to delete this transform?'))
    {
    var form = $('<form action="#" method="post">' +
  '<input type="hidden" name="deleteID" value="' + ID + '" />' +
  '</form>');
  $('body').append(form);
  form.submit();
    }
    else
    {
      return false;
    }
}









function modifyName(ID) {
    //code
    var currentVal = $('#Name' +  ID).text();
    var editField = "<textarea cols=45 class='nameTA' orig='" + escape(currentVal) + "' name='n_" + ID + "' id='nTA_" + ID + "'></textarea>";
    
    $('#Name' +  ID).empty().append(editField).focus();
    $('#Name' +  ID).find('textarea').val(currentVal).focus();
    //console.log($("#mTA_" + ID));
     //$('#mTA_' +  ID).focus().empty().val('asdasd');
    //alert("modifying mapping "+ ID);
}
$('#transformTable').on('focusout','.nameTA',function(e) {
  saveName($(this));
});





$('#transformTable').on('keydown','.nameTA',function(e) {
    
    var code = e.keyCode ? e.keyCode : e.which;
    if (code == 13) {  // Enter keycode
      //alert()
     saveName($(this));
   }
});



function saveName(nameTarget) {
  
     var rowID = $(nameTarget).attr('name').substr(2);
      var original = unescape($(nameTarget).attr('orig'));
      var now = $(nameTarget).val().replace(/ /g,"_");;
      var now = now.replace(/(\r\n|\n|\r)/gm,"");
      if (now == original || now == "" || now == "-empty") {
        //alert("same same");
        var editText = "<span onclick='modifyName(" + rowID + ");'>" + original + "</span>";
        $('#Name' +  rowID).empty().append(editText);
      }
      else
      {
        //ajax something
      $('#Name' +  rowID).append("<img src='loading.gif' alt='saving...'/>");
  $.ajax({
      url : "saveParts.php",
      type: "POST",
      data : {ID:rowID,name:now,nameUpdate:"true"},
      success: function(data, textStatus, jqXHR)
      {
          //data - response from server
          var editText = "<span onclick='modifyName(" + rowID + ");'>" + now + "</span>";
          $('#Name' +  rowID).empty().append(editText);
      },
      error: function (jqXHR, textStatus, errorThrown)
      {
   
      }
  });
      }
      
}














function modifyInput(ID) {
    //code
    var currentVal = $('#Input' +  ID).text();
    var editField = "<textarea cols=45 class='inputTA' orig='" + escape(currentVal) + "' name='i_" + ID + "' id='iTA_" + ID + "'></textarea>";
    
    $('#Input' +  ID).empty().append(editField).focus();
    $('#Input' +  ID).find('textarea').val(currentVal).focus();
    //console.log($("#mTA_" + ID));
     //$('#mTA_' +  ID).focus().empty().val('asdasd');
    //alert("modifying mapping "+ ID);
}
$('#transformTable').on('focusout','.inputTA',function(e) {
  saveInput($(this));
});





$('#transformTable').on('keydown','.inputTA',function(e) {
    
    var code = e.keyCode ? e.keyCode : e.which;
    if (code == 13) {  // Enter keycode
      //alert()
     saveInput($(this));
   }
});



function saveInput(inputTarget) {
  
     var rowID = $(inputTarget).attr('name').substr(2);
      var original = unescape($(inputTarget).attr('orig'));
      var now = $(inputTarget).val();
      var now = now.replace(/(\r\n|\n|\r)/gm,"");
      if (now == original || now == "" || now == "-empty") {
        //alert("same same");
        var editText = "<span onclick='modifyInput(" + rowID + ");'>" + original + "</span>";
        $('#Input' +  rowID).empty().append(editText);
      }
      else
      {
        //ajax something
      $('#Input' +  rowID).append("<img src='loading.gif' alt='saving...'/>");
  $.ajax({
      url : "saveParts.php",
      type: "POST",
      data : {ID:rowID,input:now,inputUpdate:"true"},
      success: function(data, textStatus, jqXHR)
      {
          //data - response from server
          var editText = "<span onclick='modifyInput(" + rowID + ");'>" + now + "</span>";
          $('#Input' +  rowID).empty().append(editText);
      },
      error: function (jqXHR, textStatus, errorThrown)
      {
   
      }
  });
      }
      
}













function modifyQuery(ID) {
    //code
    
    
       
    var currentVal = $('#Query' +  ID).text();
    
    
    
    
    
    var editField = "<textarea cols=45 class='queryTA' orig='" + escape(currentVal) + "' name='q_" + ID + "' id='qTA_" + ID + "'></textarea>";
    
    $('#Query' +  ID).empty().append(editField).focus();
    $('#Query' +  ID).find('textarea').val(currentVal).focus();
    //console.log($("#mTA_" + ID));
     //$('#mTA_' +  ID).focus().empty().val('asdasd');
    //alert("modifying mapping "+ ID);
}
$('#transformTable').on('focusout','.queryTA',function(e) {
  saveQuery($(this));
});



$('#transformTable').on('keydown','.queryTA',function(e) {
    
    var code = e.keyCode ? e.keyCode : e.which;
    if (code == 13) {  // Enter keycode
      //alert()
     saveQuery($(this));
   }
});



function saveQuery(queryTarget) {
  
     var rowID = $(queryTarget).attr('name').substr(2);
      var original = unescape($(queryTarget).attr('orig'));
      var now = $(queryTarget).val();
      var now = now.replace(/(\r\n|\n|\r)/gm,"");
      if (now == original || now == "" || now == "-empty") {
        
        var editText = "<span onclick='modifyQuery(" + rowID + ");'>" + original + "</span>";
        $('#Query' +  rowID).empty().append(editText);
      }
      else
      {
        //lets check it ends with | table
    var splitTerms = now.split("|");
    
    var lastTerm = splitTerms[splitTerms.length-1].trim().split(" ")[0];
    
    if (lastTerm.toLowerCase().indexOf('table') == -1) {
        alert("All queries must end with '| table <fields>' so that we can perform mapping. Please fix!");
        
        return;
    }
        
        
        //ajax something
      $('#Query' +  rowID).append("<img src='loading.gif' alt='saving...'/>");
  $.ajax({
      url : "saveParts.php",
      type: "POST",
      data : {ID:rowID,query:now,queryUpdate:"true"},
      success: function(data, textStatus, jqXHR)
      {
          //data - response from server
          var editText = "<span onclick='modifyQuery(" + rowID + ");'>" + now + "</span>";
          $('#Query' +  rowID).empty().append(editText);
      },
      error: function (jqXHR, textStatus, errorThrown)
      {
   
      }
  });
      }
      
}

function saveMapping(mappingTarget) {
  
     var rowID = $(mappingTarget).attr('name').substr(2);
      var original = unescape($(mappingTarget).attr('orig'));
      var now = $(mappingTarget).val();
      var now = now.replace(/(\r\n|\n|\r)/gm,"");
     
      if (now == original  || now == "" || now == "-empty") {
        //alert("same same");
        var editText = "<span onclick='modifyMapping(" + rowID + ");'>" + original + "</span>";
        $('#Mapping' +  rowID).empty().append(editText);
      }
      else
      {
        //ajax something
        $('#Mapping' +  rowID).append("<img src='loading.gif' alt='saving...'/>");
  $.ajax({
      url : "saveParts.php",
      type: "POST",
      data : {ID:rowID,mapping:now,mappingUpdate:"true"},
      success: function(data, textStatus, jqXHR)
      {
          //data - response from server
          var editText = "<span onclick='modifyMapping(" + rowID + ");'>" + now + "</span>";
          $('#Mapping' +  rowID).empty().append(editText);
      },
      error: function (jqXHR, textStatus, errorThrown)
      {
   
      }
  });
      }
      
}

function modifyMapping(ID) {
    //code
    var currentVal = $('#Mapping' +  ID).text();
    var editField = "<textarea cols=45 class='mappingTA' orig='" + escape(currentVal) + "' name='m_" + ID + "' id='mTA_" + ID + "'></textarea>";
    
    $('#Mapping' +  ID).empty().append(editField).focus();
    $('#Mapping' +  ID).find('textarea').val(currentVal).focus();
    //console.log($("#mTA_" + ID));
     //$('#mTA_' +  ID).focus().empty().val('asdasd');
    //alert("modifying mapping "+ ID);
}
$('#transformTable').on('focusout','.mappingTA',function(e) {
  saveMapping($(this));
});



$('#transformTable').on('keydown','.mappingTA',function(e) {
    
    var code = e.keyCode ? e.keyCode : e.which;
    if (code == 13) {  // Enter keycode
      //alert()
     saveMapping($(this));
   }
});

$(document).ready( function () {
      
    
    $('#transformTable').dataTable( {
  "pageLength": 10
} );

    
    
} );
