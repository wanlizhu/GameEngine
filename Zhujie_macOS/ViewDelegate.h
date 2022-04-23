#import <MetalKit/MetalKit.h>

@interface ViewDelegate : NSObject <MTKViewDelegate>

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view;

@end

