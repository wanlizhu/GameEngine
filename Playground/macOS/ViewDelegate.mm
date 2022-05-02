#import <simd/simd.h>
#import <ModelIO/ModelIO.h>
#import "ViewDelegate.h"
#import "OpenWorld/OpenWorld.h"

@implementation ViewDelegate
{
    OpenWorld::ARC _world;
}

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view;
{
    self = [super init];
    
    _world = std::make_shared<OpenWorld>();
    _world->initWithView((__bridge void*)view);
    _world->loadModel("DamagedHelmet.gltf");
    
    return self;
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
   if (_world)
   {
       _world->drawInView((__bridge void*)view);
   }
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    if (_world)
    {
        _world->resizeView((__bridge void*)view);
    }
}

@end
