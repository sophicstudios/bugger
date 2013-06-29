//
//  test_shader.fsh
//  wrangler-ios
//
//  Created by Lyell Haynes on 7/8/12.
//  Copyright (c) 2012 Sophic Studios. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
