#import "ViewController.h"
#import "ViewDelegate.h"

@implementation ViewController
{
    MTKView* _view;
    ViewDelegate* _delegate;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _view = (MTKView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();
    _view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    _view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    _view.clearColor = MTLClearColorMake(0, 0, 0, 1);
    _view.clearDepth = 1.0;

    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        self.view = [[NSView alloc] initWithFrame:self.view.frame];
        return;
    }

    _delegate = [[ViewDelegate alloc] initWithMetalKitView:_view];
    [_delegate mtkView:_view drawableSizeWillChange:_view.bounds.size];

    _view.delegate = _delegate;
}

@end
