// Get a reference to the root of the chat data.
var messagesRef = new Firebase('https://shafe.firebaseio.com/');

window.sendMessage = function(text) {
    var name = $('#nameInput').val();
    messagesRef.push({name:name, text:text});
    $('#messageInput').val('');
}

// When the user presses enter on the message input, write the message to firebase.
$('#messageInput').keypress(function (e) {
  if (e.keyCode === 13) {
    var text = $('#messageInput').val();
    sendMessage(text);
  }
});

// Add a callback that is triggered for each chat message.
messagesRef.limit(10).on('child_added', function (snapshot) {
  var message = snapshot.val();
  $('<div/>').text(message.text).prepend($('<em/>')
    .text(message.name+': ')).appendTo($('#messagesDiv'));
  $('#messagesDiv')[0].scrollTop = $('#messagesDiv')[0].scrollHeight;
});
