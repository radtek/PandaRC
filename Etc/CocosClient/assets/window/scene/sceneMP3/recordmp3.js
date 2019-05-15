let game = require("game");

cc.Class({
    extends: cc.Component,

    editor: {
    },

    properties: {
        tipsLabel: cc.Label,
        recordingSF: cc.SpriteFrame,
        recordSF: cc.SpriteFrame,
        _isRecording: false,
        videoPlayer: cc.VideoPlayer,
    },

    ctor() {
        console.log("recordmp3 construct***###");
    },

    // use this for initialization
    onLoad: function () {
       let self = this;
       self.tipsLabel.string = "loading...";
       require("recorder.mp3.min");
       var set = {
           type:"mp3" //输出类型：mp3,wav，wav输出文件尺寸超大不推荐使用，但mp3编码支持会导致js文件超大，如果不需支持mp3可以使js文件大幅减小
       }
       var rec = Recorder(set);
       function successCallback() {
            self.tipsLabel.string = "press the btn below to start recording...";
            self.node.scaleX = 1;
       }

        function errorCallback(error) {
            console.log("无法录音:"+error);
        }
        rec.open(successCallback, errorCallback);

        self.node.on("touchstart",function(){
            if(!self._isRecording){
                self.videoPlayer.stop();
                rec.start();
                self._isRecording = true;
                self.tipsLabel.string = "recording. say something please...";
                self.getComponent(cc.Sprite).spriteFrame = self.recordingSF;
                return;
            };
            
            rec.stop(function (blob, duration) {
                console.log(URL.createObjectURL(blob), "时长:"+duration+"ms");
                self.videoPlayer.remoteURL = URL.createObjectURL(blob);
                self.videoPlayer.play();
                // rec.close();
                game.httpNet.asyncPost("http://127.0.0.1:120/upload.php", blob);

                self._isRecording = false;
                self.tipsLabel.string = "press the btn below to start recording...";
                self.getComponent(cc.Sprite).spriteFrame = self.recordSF;
            }, function(error) { console.log("录音失败:"+error); });
        });
    },


    // called every frame, uncomment this function to activate update callback
    // update: function (dt) {

    // },

    start () {
    },
});
