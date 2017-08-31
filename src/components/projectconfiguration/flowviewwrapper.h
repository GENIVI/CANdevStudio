#ifndef FLOWVIEWWRAPPER_H
#define FLOWVIEWWRAPPER_H

struct FlowViewWrapper : public QtNodes::FlowView {
	    FlowViewWrapper(QtNodes::FlowScene* scene)
		        : QtNodes::FlowView(scene)
		        , _scene(scene)

    {
	        setAcceptDrops(true);
	    }

private :
	QtNodes::FlowScene* _scene;
};

#endif // FLOWVIEWWRAPPER_H
