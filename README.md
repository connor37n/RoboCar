# Robo Car
Video of Full Run:
https://www.youtube.com/watch?v=IwY8QTH6qbo

The goal of Fall Quarter’s ECE 3 course project was to design and create code on the Energia coding platform that, 
when uploaded to a provided miniature battery-powered car, would navigate a race track composed of a dark line from beginning 
to end and back within an allotted time frame. The key characteristics defining said task was the race track’s gradient, 
which was darkest at the center and lighter on the sides, as well as the car’s means of reacting to said track in the form
of six phototransistors lining the front bottom side of the vehicle. Consequently, the primary issue we were tasked with solving
in order to complete this project was translating the effect of the reflection of the track and the ground on the phototransistors
into signals that would make the car navigate the race track.

The basic theory for understanding the path sensing system revolves around the IR phototransistors on the underside of the car, 
which serve as sensors which collect data by which our code interprets and signals as changes to the speed of the car’s wheels 
in order to navigate the track.
