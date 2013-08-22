#!/usr/bin/env python
# -*- coding: utf-8 -*-

import tempfile
import shutil
import sys
from PythonQt import QtCore

class TestCase(object):
    def fail_callback(self, fail_cb):
        self._fail_cb = fail_cb

    def pass_callback(self, pass_cb):
        self._pass_cb = pass_cb

    def Passed():
        self._pass_cb()

    def Failed():
        self._fail_cb()

class TestCaseRunner(object):
    tests = []
    def __init__(self, test_list):
        self.tests = test_list
        self.exceptions = {}
        self.errors = 0
        self.fails = 0
        self.timer = QtCore.QTimer()
        self.timer.connect("timeout()", self.stop_test_timeout)

    def run_first_testcase(self):
        if not tests:
            print "passed:", self.passed
            print "failed:", self.failed
            print "errors:", self.errors
            sys.exit(0)
            return
        self.tests.pop()


    def run_next_test():



class TestRunner(object):
    def __init__(self, test_obj, endcb):
        self.end_cb = endcb
        self.test_obj = test_obj
        self.test_obj.pass_test.append(self.passTest)
        self.test_obj.fail_test.append(self.failTest)
        self.tests = [i for i in dir(test_obj) if i.startswith('test_')]
        self.exceptions = {}
        self.errors = 0
        self.passed = 0
        self.timer = QtCore.QTimer()
        self.timer.connect("timeout()", self.failTest)
        self.failed = 0

    def failTest():
        self.failed += 1
        sys.stdout.write("F")
        self.stop_test()

    def passTest():
        sys.stdout.write(".")
        self.passed += 1
        self.stop_test()

    def stop_test():
        self.timer.stop()
        self.testing.tearDown()
        self.run_next_test()

    def run_next_test(self):
        if not self.tests:
            self.end_cb(self.passed, self.failed, self.errors, self.exceptions)
            return

        self.testing = self.tests.pop(0)
        if hasattr(self.test_obj, 'setUp'):
            self.test_obj.setUp()
        self.timer.setInterval(int(self.test_obj * 1000))
        self.timer.setSingleShot(True)
        self.timer.start()
        try:
            self.testing()
        except Exception, e:
            self.exceptions[self.testing.__name__] = e
            self.errors += 1
            sys.stdout.write("E")
            self.stop_test()

