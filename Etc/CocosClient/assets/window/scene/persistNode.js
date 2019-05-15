cc.Class({
    extends: cc.Component,

    ctor: function() {
    },

    editor: {
    },

    properties: {
    },

    onLoad: function() {
        this.data = "1234";
        cc.game.addPersistRootNode(this.node);
    },
});
