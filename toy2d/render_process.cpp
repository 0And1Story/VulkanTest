/**
  * @file   render_process.cpp
  * @author 0And1Story
  * @date   2026-01-14
  * @brief  
  */

#include "render_process.h"

#include "context.h"
#include "shader.h"

namespace toy2d {

void RenderProcess::InitRenderProcess(int width, int height) {
    InitLayout();
    InitRenderPass();
    InitPipeline(width, height);
}

void RenderProcess::DestroyRenderProcess() {
    auto& device = Context::GetInstance().device;
    device.destroyPipeline(pipeline);
    device.destroyRenderPass(renderPass);
    device.destroyPipelineLayout(layout);
}

void RenderProcess::InitPipeline(int width, int height) {
    vk::GraphicsPipelineCreateInfo createInfo;

    // 1. Vertex Input
    vk::PipelineVertexInputStateCreateInfo inputState;
    createInfo.setPVertexInputState(&inputState);

    // 2. Vertex Assembly
    vk::PipelineInputAssemblyStateCreateInfo asmState;
    asmState
    .setPrimitiveRestartEnable(false) // disable primitive restart
    .setTopology(vk::PrimitiveTopology::eTriangleList);
    createInfo.setPInputAssemblyState(&asmState);

    // 3. Shader
    auto stages = Shader::GetInstance().getStages();
    createInfo.setStages(stages);

    // 4. Viewport State
    vk::PipelineViewportStateCreateInfo viewportState;
    vk::Viewport viewport(0, 0, width, height, 0, 1);
    vk::Rect2D scissor({0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
    viewportState
    .setViewports(viewport)
    .setScissors(scissor);
    createInfo.setPViewportState(&viewportState);

    // 5. Rasterization
    vk::PipelineRasterizationStateCreateInfo rasterState;
    rasterState
    .setRasterizerDiscardEnable(false) // if true then no output to framebuffer
    .setCullMode(vk::CullModeFlagBits::eBack)
    .setFrontFace(vk::FrontFace::eCounterClockwise)
    .setPolygonMode(vk::PolygonMode::eFill)
    .setLineWidth(1); // no depth settings needed for 2D
    createInfo.setPRasterizationState(&rasterState);

    // 6. Multisample
    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample
    .setSampleShadingEnable(false) // disable multisampling
    .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    createInfo.setPMultisampleState(&multisample);

    // 7. Test (No need for 2D)

    // 8. Color Blending
    vk::PipelineColorBlendStateCreateInfo blend;
    vk::PipelineColorBlendAttachmentState attach;
    attach
    .setBlendEnable(false)
    .setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB);
    blend
    .setLogicOpEnable(false) // disable logic operations
    .setAttachments(attach);
    createInfo.setPColorBlendState(&blend);

    // 9. Layout
    createInfo.setLayout(layout);

    // 10. Render Pass
    createInfo.setRenderPass(renderPass);

    auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr, createInfo);
    if (result.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create graphics pipeline.");
    }
    pipeline = result.value;
}

void RenderProcess::InitLayout() {
    vk::PipelineLayoutCreateInfo createInfo;
    // No layout needed currently
    layout = Context::GetInstance().device.createPipelineLayout(createInfo);
}

void RenderProcess::InitRenderPass() {
    vk::RenderPassCreateInfo createInfo;

    // Attachments
    vk::AttachmentDescription attachDesc;
    attachDesc
    .setFormat(Context::GetInstance().swapchain->info.format.format)
    .setInitialLayout(vk::ImageLayout::eUndefined)
    .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
    .setLoadOp(vk::AttachmentLoadOp::eClear)
    .setStoreOp(vk::AttachmentStoreOp::eStore)
    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare) // no stencil currently
    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
    .setSamples(vk::SampleCountFlagBits::e1);
    createInfo.setAttachments(attachDesc);

    // Subpasses
    vk::AttachmentReference attachRef;
    attachRef
    .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
    .setAttachment(0); // only one attachment, index = 0
    vk::SubpassDescription subpass;
    subpass
    .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics) // graphics subpass
    .setColorAttachments(attachRef); // 1+ color, 0+ depth&stencil
    createInfo.setSubpasses(subpass);

    // Dependencies
    vk::SubpassDependency dependency;
    dependency
    .setSrcSubpass(VK_SUBPASS_EXTERNAL) // the vk init subpass
    .setDstSubpass(0) // our subpass index
    .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
    .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
    .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    createInfo.setDependencies(dependency);

    renderPass = Context::GetInstance().device.createRenderPass(createInfo);
}

}