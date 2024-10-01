function HandleButton(evt) {
    alert("You clicked the button!");
  // Call our C++ callback 'GetMessage'
  //var message = GetFileInfos("aAYou clicked the button!".split('').map(char => char.charCodeAt(0)));
  var message = GetMessage();

  // Display the result in our 'message' div element and apply the
  // rainbow effect to our document's body.
  document.getElementById('message').innerHTML = message;
  document.body.classList.add('rainbow');
}
