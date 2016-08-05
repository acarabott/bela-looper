~bela = NetAddr("192.168.7.2", 6666);
~bela.sendMsg("/tempo", 65.0);



(
if(w.notNil) {w.close};
w = Window("record", Rect(100, 100, 500, 300)).front;
~buttons = 10.collect {|i|
  Button(w, Rect(i * 50,0,40,40))
    .states_([
      [i + 1, Color.black, Color.white],
      [i + 1, Color.white, Color.black]
    ])
    .action_({|butt|
      var record = [false, true][butt.value].postln;
      var quantise = true;
      ~bela.sendMsg("/record", i, record, quantise);
    });
};

w.view.keyDownAction_({|view, char, mod, uni, key|
  char.isDecDigit && {
    var digit = char.digit;
    var channel = if(digit == 0) {channel = 9} { channel = digit - 1};
    var button = ~buttons[channel];
    if(button.notNil) {
      button.valueAction_((button.value - 1).abs);
    };
  }
});
)
