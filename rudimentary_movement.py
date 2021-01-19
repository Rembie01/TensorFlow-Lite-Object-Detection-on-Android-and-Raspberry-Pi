# Author: Rembie01
# Date: 8 dec 2020


class Movement:
    """Allows the stepper motor to move"""

    def __init__(self, factor=1, fov_angle_horizontal=62, fov_angle_vertical=37, resolution=(1280, 720), play=1):
        self.factor = factor
        self.h_angle = fov_angle_horizontal
        self.v_angle = fov_angle_vertical
        self.res = resolution
        self.play = play

    def move_horizontal(self, xcoord):
        rotate_pixels = -(xcoord - self.res[0] / 2)
        pixels_per_degree = self.res[0] / self.h_angle
        rotate = round(rotate_pixels / pixels_per_degree)
        if abs(rotate) <= self.play:
            return '<00,00,0000>'
        rpm = abs(round(self.factor * rotate))
        return '<00,' + str(rotate) + ',' + str(rpm) + '>'

    def move_vertical(self, ycoord):
        tilt_pixels = -(ycoord - self.res[1] / 2)
        pixels_per_degree = self.res[1] / self.v_angle
        tilt = round(tilt_pixels / pixels_per_degree)
        if abs(tilt) <= self.play:
            return '<00,00,0000>'
        rpm = abs(round(self.factor * tilt))
        return '<' + str(tilt) + ',00,' + str(rpm) + '>'

    def move(self, xcoord, ycoord):
        rotate_pixels = -(xcoord - self.res[0] / 2)
        tilt_pixels = -(ycoord - self.res[1] / 2)
        rotate_pixels_per_degree = self.res[0] / self.h_angle
        tilt_pixels_per_degree = self.res[1] / self.v_angle
        rotate = round(rotate_pixels / rotate_pixels_per_degree)
        tilt = round(tilt_pixels / tilt_pixels_per_degree)
        if abs(rotate) <= self.play and abs(tilt) <= self.play:
            return '<0,0,0,0>'
        if abs(rotate) <= self.play:
            tilt_rpm = abs(round(self.factor * tilt))
            return '<' + str(tilt) + ',0,' + str(tilt_rpm) + ',0>'
        if abs(tilt) <= self.play:
            rotate_rpm = abs(round(self.factor * rotate))
            return '<0,' + str(rotate) + ',0,' + str(rotate_rpm) + '>'
        rotate_rpm = abs(round(self.factor * rotate))
        tilt_rpm = abs(round(self.factor * tilt))
        return '<' + str(tilt) + ',' + str(rotate) + ',' + str(tilt_rpm) + ',' + str(rotate_rpm) + '>'



