# LarvaeTracker

Tracking suite. Specialized for use with FTIR-like data and tracking of Drosophila larvae.
Automated tracking via different algorithms (extendable through a basic plugin-system) as well as manual tracking through a simple GUI.

Requires OpenCV 2.4.1 and QT 4.8.4

![Screenshot of result-view](http://exean.net/larvae/screenshot.png)
![Tracking results](http://exean.net/larvae/result.JPG)

## Model representation of a larva

Each larva is represented as an ordered list of points v with radius b. The number N of points can be set freely to achieve the desired number of segments of a virtual larva-spine S'. The confirmation of a larva is represented through the angles φ at each point v.

![Model representation of a larva](http://exean.net/larvae/model.jpg)

A model-based energy-projection visualizes expected movement of a larvae.

![Energy projection](http://exean.net/larvae/energy.png)


## Collisions and Separation

![Colliding larvae](http://exean.net/larvae/tkollision.png)

Differents algorithms are available to separate colliding larvae. Manual separation via the GUI is also possible.

![Separation of colliding larvae 1](http://exean.net/larvae/gt63.PNG)
![Separation of colliding larvae 0](http://exean.net/larvae/gt128.PNG)
