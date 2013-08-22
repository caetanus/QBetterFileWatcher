#!/usr/bin/env python
# -*- coding: utf-8 -*-


class FileWatcherTestCase(TestBase):
    def setUp(self):
        self.test_dir = tempfile.mkdtemp("filewatcher")
        self.watcher = Watcher(self.test_dir)
        self.watcher.connect("genericEvent(QString, QString)", self._event_processor)
        self.watcher.connect("genericEvent(QString, QString, QString)",
                             self._event_processor)
        self.timeout = 0.5 # time in seconds that any test will fail if it not yet passed.

    def tearDown(self):
        self.watcher.disconnect(self._event_processor)
        del self.watcher
        shutil.rmtree(self.test_dir)

    def test_create
