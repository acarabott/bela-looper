~bela = NetAddr("192.168.7.2", 6666);
~bela.sendMsg("/tempo", 80);


(
~numChannels = 10;
if(w.notNil) {w.close};
w = Window("record", Rect(100, 100, 500, 350)).front;
w.view.addFlowLayout;

~recordButtons = ~numChannels.collect {|i|
  var label = "R %".format(i + 1);
  Button(w, Rect(i * 50,0,40,40))
    .states_([
      [label, Color.black, Color.white],
      [label, Color.white, Color.black]
    ])
    .action_({|butt|
      var record = [false, true][butt.value].postln;
      var quantise = true;
      ~bela.sendMsg("/record", i, record, quantise);
    });
};

w.view.decorator.nextLine;

~eraseButtons = ~numChannels.collect {|i|
  Button(w, Rect(i * 50,0,40,40))
    .states_([
      ["E %".format(i + 1), Color.white, Color.red],
    ])
    .action_({|butt|
      ~bela.sendMsg("/erase", i);
    });
};

w.view.decorator.nextLine;

~sliders = ~numChannels.collect {|i|
  EZSlider(w, Rect(i * 50, 0, 40, 200), label:"V%".format(i + 1), layout: \vert)
    .value_(1)
    .action_({|ez|
      ~bela.sendMsg("/volume", i, ez.value);
    });
};

w.view.decorator.nextLine;

~muteButtons = ~numChannels.collect {|i|
  var label = "M %".format(i + 1);
  Button(w, Rect(i * 50,0,40,40))
    .states_([
      [label, Color.white, Color.blue(1.0, 0.2)],
      [label, Color.white, Color.blue(1.0, 0.8)]
    ])
    .action_({|butt|
      ~bela.sendMsg("/volume", i, (1 - butt.value).abs.asFloat);
    });
};

w.view.keyDownAction_({|view, char, mod, uni, key|
  char.isDecDigit && {
    var digit = char.digit;
    var channel = if(digit == 0) {channel = 9} { channel = digit - 1};
    var button = ~recordButtons[channel];
    if(button.notNil) {
      button.valueAction_((button.value - 1).abs);
    };
  }
});
)
