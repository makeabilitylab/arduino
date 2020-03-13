---
layout: default
title: Microcontrollers
nav_order: 2
has_children: true
# has_toc: false (on by default)
---
# Microcontrollers
{: .no_toc }

## Table of Contents
{: .no_toc .text-delta }

1. TOC
{:toc}
---
This is the parent page for arduino!

# Things to add
- The basics of microcontrollers
- Digital Input
- Digital Output
- Analog Input
- Analog Output
  - PWM
- Interrupts
- Digital Com Interfaces
  - I2C
  - SPI
- Debugging (but where should this go?)
  - Serial.print and Serial plotter and console
  - Multimeters
- Using LEDs
- Using RGB LEDs

<!-- Code snippet highlighting: https://jekyllrb.com/docs/liquid/tags/#code-snippet-highlighting -->
{% highlight C %}
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}
{% endhighlight C %}

# Test Code Snippets
<!-- <script src="http://gist-it.appspot.com/http://github.com/$file"></script> -->
This is awesome! Can embed code directly!
<script src="http://gist-it.appspot.com/https://github.com/jonfroehlich/physcomp/blob/master/Basics/digitalWrite/Blink/Blink.ino?footer=minimal"></script>
