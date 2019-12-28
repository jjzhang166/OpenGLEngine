#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author: penghuailiang
# @Date  : 12/29/19

from vector import Vector3, CubeUV
import math
import random


def Cartesian2Spherical(p):
    """
    :type p: Vector3
    """
    s = Vector3.zero()
    s.radius = p.magnitude()
    s.theta = math.acos(p.y / s.radius)
    s.phi = math.atan2(p.z, p.x)
    return s


def Spherical2Cartesian(s):
    """
    :type s: Vector3
    """
    p = Vector3.zero()
    p.x = s.radius * math.sin(s.theta) * math.cos(s.phi)
    p.y = s.radius * math.cos(s.theta)
    p.z = s.radius * math.sin(s.theta) * math.sin(s.phi)
    return p


def CubeUV2XYZ(c):
    """
    :type c: CubeUV
    :return: Position
    """
    u = c.u * 2.0 - 1.0
    v = c.v * 2.0 - 1.0
    if c.index == 0:
        return Vector3(1, v, u)
    elif c.index == 1:
        return Vector3(-1, v, u)
    elif c.index == 2:
        return Vector3(u, 1, -v)
    elif c.index == 3:
        return Vector3(u, -1, v)
    elif c.index == 4:
        return Vector3(u, v, 1)
    elif c.index == 5:
        return Vector3(-u, v, -1)
    else:
        return Vector3.zero()


def XYZ2CubeUV(p):
    """
    :type p: Vector3
    :return type: CubeUV
    """
    ax = abs(p.x)
    ay = abs(p.y)
    az = abs(p.z)
    if ax >= ay and ax >= az:
        c = CubeUV(0 if p.x >= 0 else 0, -p.z / p.x, p.y / ax)
    elif ay >= az:
        c = CubeUV(2 if p.y >= 0 else 3, p.x / ay, -p.z / p.y)
    else:
        c = CubeUV(4 if p.z > 0 else 5, p.x / p.z, p.y / az)
    c.u = c.u * 0.5 + 0.5
    c.v = c.v * 0.5 + 0.5
    return c


def NormalRandom(mu=0, sigma=1):
    """
    生成服从正态分布的随机数
    :param mu: 均值
    :param sigma: 标准差
    """
    return random.normalvariate(mu, sigma=sigma)
